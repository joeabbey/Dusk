#include "dusk/App.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>

#include <fstream>

namespace dusk {

App * App::_Inst = nullptr;

App::App(int argc, char** argv)
{
    App::InitScripting();

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

    if (!glfwInit())
    {
        DuskLogError("Failed to initialize GLFW");
        return;
    }

    glfwSetErrorCallback(&App::GLFW_ErrorCallback);

#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 16);
    _glfwWindow = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), NULL, NULL);
    if (!_glfwWindow)
    {
        DuskLogError("Failed to create GLFW window");
        return;
    }

    glfwMakeContextCurrent(_glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        DuskLogError("Failed to initialize OpenGL context");
        return;
    }

    int samples;
    glGetIntegerv(GL_SAMPLES, &samples);
    DuskLogInfo("Running %dx AA", samples);

    ImGui_ImplGlfwGL3_Init(_glfwWindow, false);

    glfwSetMouseButtonCallback(_glfwWindow, &App::GLFW_MouseButtonCallback);
    glfwSetScrollCallback(_glfwWindow, &App::GLFW_ScrollCallback);
    glfwSetKeyCallback(_glfwWindow, &App::GLFW_KeyCallback);
    glfwSetCharCallback(_glfwWindow, &App::GLFW_CharCallback);

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
    ImGui_ImplGlfwGL3_Shutdown();

    glfwDestroyWindow(_glfwWindow);
    _glfwWindow = nullptr;

    glfwTerminate();
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

    if (_glfwWindow)
    {
        glfwSetWindowSize(_glfwWindow, WindowWidth, WindowHeight);
        glfwSetWindowTitle(_glfwWindow, WindowTitle.c_str());
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

        Shader * dusk_shader = new Shader(shader["Name"], shader["BindData"], shaderFiles);
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
            DuskLogInfo("Read Actor %s", actor["Name"].get<std::string>().c_str());
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
                    dusk_comp = new MeshComponent(dusk_actor, new FileMesh(_shaders[shader], comp["File"].get<std::string>()));
                }
                else if ("Script" == type)
                {
                    dusk_comp = new ScriptComponent(dusk_actor, comp["File"].get<std::string>());
                }
                else if ("Camera" == type)
                {
                    Camera * dusk_camera = new Camera();
                    dusk_scene->SetCamera(dusk_camera);

                    dusk_camera->SetPosition({
                        comp["Position"][0], comp["Position"][1], comp["Position"][2]
                    });
                    dusk_camera->SetForward({
                        comp["Forward"][0], comp["Forward"][1], comp["Forward"][2]
                    });

                    dusk_comp = new CameraComponent(dusk_actor, dusk_camera);
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
        _startScene = startScene->second;
    }

    file.close();

    DuskBenchEnd("App::LoadConfig()");
    return true;
}

void App::PushScene(Scene * scene)
{
    if (!scene) return;

    if (!_sceneStack.empty())
    {
        _sceneStack.top()->Free();
    }

    _sceneStack.push(scene);

    DuskLogInfo("Starting Scene %s", _sceneStack.top()->GetName().c_str());
    _sceneStack.top()->Load();
}

void App::PopScene()
{
    if (_sceneStack.empty()) return;

    _sceneStack.top()->Free();
    _sceneStack.pop();

    if (_sceneStack.empty()) return;

    DuskLogInfo("Starting Scene %s", _sceneStack.top()->GetName().c_str());
    _sceneStack.top()->Load();
}

void App::Run()
{
    CreateWindow();

    // Load the shaders, now that we have a GL Context
    for (const auto& it : _shaders)
    {
        it.second->Load();
    }

    // Load the starting scene
    PushScene(_startScene);

    double frame_delay = 1.0;   // MS until next frame
    double frame_elap  = 0.0;   // MS since last frame
    double fps_delay   = 250.0; // MS until next FPS update
    double fps_elap    = 0.0;   // MS since last FPS update
    double elapsed;

    unsigned long frames = 0;

    UpdateEventData updateEventData;

    updateEventData.SetTargetFPS(TARGET_FPS);
    frame_delay = (1000.0 / TARGET_FPS) / 1000.0;

    double timeOffset = glfwGetTime();
    while (!glfwWindowShouldClose(_glfwWindow))
    {
        // TODO: Cleanup
        elapsed = glfwGetTime() - timeOffset;
        timeOffset = glfwGetTime();

        glfwPollEvents();

        updateEventData.Update(elapsed);
        DispatchEvent(Event((EventID)Events::UPDATE, updateEventData));

        frame_elap += elapsed;
        if (frame_delay <= frame_elap)
        {
            frame_elap = 0.0;
            ++frames;

            ImGui_ImplGlfwGL3_NewFrame();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            DispatchEvent(Event((EventID)Events::RENDER));

            UI::Render();

            glfwSwapBuffers(_glfwWindow);
        }

        fps_elap += elapsed;
        if (fps_delay <= fps_elap)
        {
            float fps = (float)((frames / fps_elap) * 1000.0f);
            updateEventData.SetCurrentFPS(fps);

            frames = 0;
            fps_elap = 0.0;
        }
    }

    PopScene();

    DestroyWindow();
}

void App::InitScripting()
{
    ScriptHost::AddFunction("dusk_App_GetInst", &App::Script_GetInst);
    ScriptHost::AddFunction("dusk_App_LoadConfig", &App::Script_LoadConfig);
    ScriptHost::AddFunction("dusk_App_GetScene", &App::Script_GetScene);

    IEventDispatcher::InitScripting();

    Scene::InitScripting();
    Actor::InitScripting();
    //Component::InitScripting();
    //MeshComponent::InitScripting();
    //CameraComponent::InitScripting();
    //ScriptComponent::InitScripting();
}

int App::Script_GetInst(lua_State * L)
{
    lua_pushinteger(L, (ptrdiff_t)App::GetInst());

    return 1;
}

int App::Script_LoadConfig(lua_State * L)
{
    App * app = (App *)lua_tointeger(L, 1);

    bool result = app->LoadConfig(std::string(lua_tostring(L, 2)));
    lua_pushboolean(L, result);

    return 1;
}

int App::Script_GetScene(lua_State * L)
{
    App * app = (App *)lua_tointeger(L, 1);

    lua_pushinteger(L, (ptrdiff_t)app->GetScene());

    return 1;
}

void App::GLFW_ErrorCallback(int code, const char * message)
{
    DuskLogError("GLFW: %d, %s", code, message);
}

void App::GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
}

void App::GLFW_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
}

void App::GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
        UI::ConsoleShown ^= 1;

    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
}

void App::GLFW_CharCallback(GLFWwindow* window, unsigned int c)
{
    ImGui_ImplGlfwGL3_CharCallback(window, c);
}

int UpdateEventData::PushToLua(lua_State * L) const
{
    lua_newtable(L);

    lua_pushstring(L, "Delta");
    lua_pushnumber(L, _delta);
    lua_settable(L, -3);

    lua_pushstring(L, "ElapsedTime");
    lua_pushnumber(L, _elapsed_time);
    lua_settable(L, -3);

    lua_pushstring(L, "TotalTime");
    lua_pushnumber(L, _total_time);
    lua_settable(L, -3);

    lua_pushstring(L, "CurrentFPS");
    lua_pushnumber(L, _current_fps);
    lua_settable(L, -3);

    return 1;
}

void UpdateEventData::Update(double elapsed)
{
    _elapsed_time = elapsed;
    _total_time += elapsed;
    _delta = (float)(elapsed / (1000.0f / _target_fps));
}

} // namespace dusk
