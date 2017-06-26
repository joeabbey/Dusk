#include "dusk/App.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>
#include <fstream>
#include <memory>

namespace dusk {

App * App::_Inst = nullptr;

App::App(int argc, char** argv)
    : _textureCache(new AssetCache<Texture>())
    , _textureIndex(new AssetIndex<Texture>())
    , _meshCache(new AssetCache<Mesh>())
    , _meshIndex(new AssetIndex<Mesh>())
    , _materialCache(new AssetCache<Material>())
    , _materialIndex(new AssetIndex<Material>())
{
    App::InitScripting();

    DuskLogInfo("Starting Application");

    _Inst = this;

    CreateWindow();
}

App::~App()
{
    DuskLogInfo("Stopping Application");

    DestroyWindow();
}

void App::CreateWindow()
{
    DuskBenchStart();

    _alDevice = alcOpenDevice(NULL);
    _alContext = alcCreateContext(_alDevice, NULL);
    alcMakeContextCurrent(_alContext);

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
    glfwWindowHint(GLFW_VISIBLE, false);
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

    alcDestroyContext(_alContext);
    alcCloseDevice(_alDevice);
}

bool App::ParseWindow(nlohmann::json& data)
{
	if (data.find("Width") != data.end())
	{
		WindowWidth = data["Width"];
	}

	if (data.find("Height") != data.end())
	{
		WindowHeight = data["Height"];
	}

	if (data.find("Title") != data.end())
	{
		WindowTitle = data["Title"].get<std::string>();
	}

	if (_glfwWindow)
	{
		glfwSetWindowSize(_glfwWindow, WindowWidth, WindowHeight);
		glfwSetWindowTitle(_glfwWindow, WindowTitle.c_str());
	}

	return true;
}

void App::LoadConfig(const std::string& filename)
{
    DuskBenchStart();

    std::ifstream file(filename);
    nlohmann::json data;

    DuskLogInfo("Loading config file '%s'", filename.c_str());

    if (!file.is_open())
    {
        DuskLogError("Failed to open config file '%s'", filename.c_str());
        return;
    }

	data << file;

	if (data.find("Window") != data.end())
	{
		ParseWindow(data["Window"]);
	}

    for (auto& shader : data["Shaders"])
    {
        _shaders[shader["ID"]] = Shader::Parse(shader);
    }

    if (data.find("DefaultScene") != data.end())
    {
        std::string sceneFilename = data["DefaultScene"].get<std::string>();
        DuskLogInfo("Loading scene config file '%s'", sceneFilename.c_str());

        std::ifstream sceneFile(sceneFilename);
        nlohmann::json scene;

        if (!sceneFile.is_open())
        {
            DuskLogError("Failed to open scene file '%s'", sceneFilename.c_str());
            return;
        }

        scene << sceneFile;
        _scene.reset(nullptr);
        _scene = Scene::Parse(scene);

        sceneFile.close();
    }

    file.close();

    DuskBenchEnd("App::LoadConfig()");
}

void App::Run()
{
    glfwShowWindow(_glfwWindow);

    DispatchEvent(Event((EventID)App::Events::START));

    double frame_delay = 1.0;   // MS until next frame
    double frame_elap  = 0.0;   // MS since last frame
    double fps_delay   = 250.0; // MS until next FPS update
    double fps_elap    = 0.0;   // MS since last FPS update
    double elapsed;

    unsigned long frames = 0;

    UpdateEventData updateEventData;

    updateEventData.SetTargetFPS(TARGET_FPS);
    frame_delay = (1000.0 / TARGET_FPS) / 1000.0;

    if (_scene)
    {
        _scene->Start();
    }

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

    DispatchEvent(Event((EventID)App::Events::STOP));

    if (!_scene)
    {
        _scene->Stop();
    }

    glfwHideWindow(_glfwWindow);
}

void App::InitScripting()
{
    ScriptHost::AddFunction("dusk_App_GetInst", &App::Script_GetInst);
    ScriptHost::AddFunction("dusk_App_LoadConfig", &App::Script_LoadConfig);
    ScriptHost::AddFunction("dusk_App_GetScene", &App::Script_GetScene);

    IEventDispatcher::InitScripting();

    Scene::InitScripting();
    Actor::InitScripting();
    Component::InitScripting();
}

int App::Script_GetInst(lua_State * L)
{
    lua_pushinteger(L, (ptrdiff_t)App::GetInst());

    return 1;
}

int App::Script_LoadConfig(lua_State * L)
{
    App * app = (App *)lua_tointeger(L, 1);

    app->LoadConfig(std::string(lua_tostring(L, 2)));

    return 0;
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
