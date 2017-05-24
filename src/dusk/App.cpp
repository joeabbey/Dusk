#include "dusk/App.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>

#include <fstream>

namespace dusk {

App * App::_Inst = nullptr;

App::App(int argc, char** argv)
{
    DuskLogInfo("Starting Application");

    _Inst = this;
}

App::~App()
{
    DuskLogInfo("Stopping Application");

    for (auto& it : _scenes)
    {
        delete it.second;
    }
    _scenes.clear();
}

void App::CreateWindow()
{
    DuskBenchStart();

    if (0 > SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER))
    {
        DuskLogError("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    _sdlWindow = SDL_CreateWindow(WindowTitle.c_str(),
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    WindowWidth, WindowHeight, SDL_WINDOW_OPENGL);

    int flags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#ifndef NDEBUG
    flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, flags);

    _sdlContext = SDL_GL_CreateContext(_sdlWindow);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        DuskLogError("Failed to initialize OpenGL context");
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    ImGui_ImplSdlGL3_Init(_sdlWindow);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    DuskBenchEnd("App::CreateWindow()");
}

void App::DestroyWindow()
{
    ImGui_ImplSdlGL3_Shutdown();

    SDL_DestroyWindow(_sdlWindow);
    _sdlWindow = NULL;

    SDL_GL_DeleteContext(_sdlContext);
    _sdlContext = NULL;

    IMG_Quit();

    SDL_Quit();
}

bool App::LoadConfig(const std::string& filename)
{
    DuskBenchStart();

    std::ifstream file(filename);
    nlohmann::json data;
    data << file;

    DuskLogInfo("Loading config file '%s'", filename.c_str());

    if (!file.is_open())
    {
        DuskLogError("Failed to open config file '%s'", filename.c_str());
        return false;
    }

    WindowWidth = data["Window"]["Width"];
    WindowHeight = data["Window"]["Height"];
    WindowTitle = data["Window"]["Title"];

    if (_sdlWindow)
    {
        SDL_SetWindowSize(_sdlWindow, WindowWidth, WindowHeight);
        SDL_SetWindowTitle(_sdlWindow, WindowTitle.c_str());
    }

    for (auto& shader : data["Shaders"])
    {
        std::vector<Shader::FileInfo> shaderFiles;

        for (auto& shaderFile : shader["Files"])
        {
            GLenum shaderType = GL_INVALID_ENUM;

            const std::string& type = shaderFile["Type"];
            if ("Vertex" == type)
            {
                shaderType = GL_VERTEX_SHADER;
            }
            else if ("Fragment" == type)
            {
                shaderType = GL_FRAGMENT_SHADER;
            }
            else if ("Geometry" == type)
            {
                shaderType = GL_GEOMETRY_SHADER;
            }
            // Compute Shader, GL 4.3+
            /*
            else if ("Compute" == type)
            {
                shaderType = GL_COMPUTE_SHADER;
            }
            */
            // Tessellation Shaders, GL 4.0+
            /*
            else if ("TessControl" == type)
            {
                shaderType = GL_TESS_CONTROL_SHADER;
            }
            else if ("TessEvaluation" == type)
            {
                shaderType = GL_TESS_EVALUATION_SHADER;
            }
            */

            shaderFiles.push_back({
                shaderType,
                shaderFile["File"],
            });
        }

        Shader * dusk_shader = new Shader(shader["Name"], shaderFiles);
        _shaders.emplace(shader["Name"], dusk_shader);
    }

    for (auto& scene : data["Scenes"])
    {
        Scene * dusk_scene = new Scene(scene["Name"].get<std::string>());
        _scenes.emplace(scene["Name"], dusk_scene);

        for (auto& camera : scene["Cameras"])
        {
            Camera * dusk_camera = new Camera();
            dusk_scene->AddCamera(dusk_camera);

            dusk_camera->SetPosition({
                camera["Position"][0], camera["Position"][1], camera["Position"][2]
            });
            dusk_camera->SetForward({
                camera["Forward"][0], camera["Forward"][1], camera["Forward"][2]
            });
        }

        for (auto& actor : scene["Actors"])
        {
            Actor * dusk_actor = new Actor(dusk_scene, actor["Name"].get<std::string>());
            dusk_scene->AddActor(dusk_actor);

            dusk_actor->SetPosition({
                actor["Position"][0], actor["Position"][1], actor["Position"][2]
            });

            for (auto& comp : actor["Components"])
            {
                Component * dusk_comp = nullptr;

                const std::string& type = comp["Type"];
                if ("Mesh" == type)
                {
                    const std::string& shader = comp["Shader"];
                    dusk_comp = new MeshComponent(dusk_actor, comp["File"].get<std::string>(), _shaders[shader]);
                }
                else if ("Script" == type)
                {
                    dusk_comp = new ScriptComponent(dusk_actor, comp["File"].get<std::string>());
                }

                if (nullptr != comp)
                {
                    dusk_actor->AddComponent(dusk_comp);
                }
            }
        }
    }

    const auto& startScene = _scenes.find(data["StartScene"]);
    if (startScene != _scenes.end())
    {
        _currentScene = startScene->second;
        DuskLogInfo("Starting Scene %s", _currentScene->GetName().c_str());
    }

    file.close();

    DuskBenchEnd("App::LoadConfig()");
    return true;
}

void App::Run()
{
    CreateWindow();

    // Load the shaders, now that we have a GL Context
    for (const auto& it : _shaders)
    {
        it.second->Load();
    }

    if (_currentScene)
    {
        _currentScene->Load();
    }

    bool running = true;
    while(running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            if (SDL_QUIT == event.type)
            {
                running = false;
                break;
            }
            if (SDL_KEYDOWN == event.type)
            {
                if (SDLK_F2 == event.key.keysym.sym)
                {
                    UI::ConsoleShown ^= 1;
                }
            }
        }

        if (_currentScene)
        {
            _currentScene->Update();
        }

        ImGui_ImplSdlGL3_NewFrame(_sdlWindow);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        UI::Render();

        if (_currentScene)
        {
            _currentScene->Render();
        }

        SDL_GL_SwapWindow(_sdlWindow);
    }

    if (_currentScene)
    {
        _currentScene->Free();
    }

    DestroyWindow();
}

} // namespace dusk
