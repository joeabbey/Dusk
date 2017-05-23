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

    for (Scene * scene : _scenes)
    {
        delete scene;
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

    _sdlWindow = SDL_CreateWindow(WindowTitle.c_str(),
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    WindowWidth, WindowHeight, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    _sdlContext = SDL_GL_CreateContext(_sdlWindow);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        DuskLogError("Failed to initialize OpenGL context");
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
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

    for (auto& scene : data["Scenes"])
    {
        Scene * dusk_scene = new Scene(scene["Name"].get<std::string>());

        for (auto& actor : scene["Actors"])
        {
            Actor * dusk_actor = new Actor(actor["Name"].get<std::string>());
            dusk_actor->SetPosition({
                actor["Position"][0], actor["Position"][1], actor["Position"][2]
            });

            for (auto& comp : actor["Components"])
            {
                Component * dusk_comp = nullptr;

                const std::string& type = comp["Type"];
                if ("Mesh" == type)
                {
                    dusk_comp = new MeshComponent(comp["File"].get<std::string>());
                }
                else if ("Script" == type)
                {
                    dusk_comp = new ScriptComponent(comp["File"].get<std::string>());
                }

                if (nullptr != comp)
                {
                    dusk_actor->AddComponent(dusk_comp);
                }
            }
            dusk_scene->AddActor(dusk_actor);
        }
        _scenes.push_back(dusk_scene);
    }

    if (_sdlWindow)
    {
        SDL_SetWindowSize(_sdlWindow, WindowWidth, WindowHeight);
        SDL_SetWindowTitle(_sdlWindow, WindowTitle.c_str());
    }

    file.close();

    DuskBenchEnd("App::LoadConfig()");
    return true;
}

void App::Run()
{
    CreateWindow();

    if (!_scenes.empty())
    {
        _currentScene = _scenes.front();
    }

    if (NULL != _currentScene)
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

        if (NULL != _currentScene)
        {
            _currentScene->Update();
        }

        ImGui_ImplSdlGL3_NewFrame(_sdlWindow);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        UI::Render();

        if (NULL != _currentScene)
        {
            _currentScene->Render();
        }

        SDL_GL_SwapWindow(_sdlWindow);
    }

    if (NULL != _currentScene)
    {
        _currentScene->Free();
    }

    DestroyWindow();
}

} // namespace dusk
