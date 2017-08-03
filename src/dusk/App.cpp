#include "dusk/App.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>
#include <dusk/Shader.hpp>
#include <fstream>
#include <memory>
#include <thread>

namespace dusk {

std::function<void(int, int, int, int)> App::_KeyFunc;
std::function<void(int, int, int)>      App::_MouseButtonFunc;
std::function<void(double, double)>     App::_MouseMoveFunc;
std::function<void(double, double)>     App::_ScrollFunc;
std::function<void(unsigned int)>       App::_CharFunc;
std::function<void(int, const char **)> App::_DropFunc;

App::App(int argc, char** argv)
{
    DuskLogInfo("Starting Application");

    _KeyFunc = [=](int key, int scancode, int action, int mods) {

        if (GLFW_PRESS == action)
        {
            OnKeyPress.Call(key, mods);
        }
        else if (GLFW_RELEASE == action)
        {
            OnKeyRelease.Call(key, mods);
        }
    };

    _MouseButtonFunc = [=](int button, int action, int mods) {
        if (GLFW_PRESS == action)
        {
            OnMousePress.Call(button, mods);
        }
        else if (GLFW_RELEASE == action)
        {
            OnMouseRelease.Call(button, mods);
        }
    };

    _MouseMoveFunc = [=](double x, double y) {
        static glm::vec2 last = { x, y };
        glm::vec2 cur = { x, y };
        OnMouseMove.Call(cur, cur - last);
        last = cur;
    };

    _ScrollFunc = [=](double xoffset, double yoffset) {
        OnMouseScroll.Call(glm::vec2(xoffset, yoffset));
    };

    _DropFunc = [=](int count, const char ** filenames) {
        std::vector<std::string> filenameList;
        for (int i = 0; i < count; ++i) filenameList.push_back(std::string(filenames[i]));
        OnFileDrop.Call(filenameList);
    };

    CreateWindow();
}

App::~App()
{
    DuskLogInfo("Stopping Application");

    DestroyWindow();
}

void App::Start()
{
    using namespace std::chrono;
    typedef duration<double, std::milli> double_ms;

    glfwShowWindow(_glfwWindow);

    OnStart.Call();

    UpdateContext updateCtx;
    RenderContext renderCtx;

    unsigned long frames = 0;

    double_ms frameDelay = 1000ms / _targetFps;
    double_ms fpsDelay = 250ms; // Update FPS 4 times per second

    double_ms frameElap = 0ms;
    double_ms fpsElap = 0ms;

    auto timeOffset = high_resolution_clock::now();

    updateCtx.TargetFPS = _targetFps;

    _running = true;
    while (_running && !glfwWindowShouldClose(_glfwWindow))
    {
        auto elapsedTime = duration_cast<double_ms>(high_resolution_clock::now() - timeOffset);
        timeOffset = high_resolution_clock::now();

        glfwPollEvents();

        updateCtx.DeltaTime = duration_cast<double_ms>(elapsedTime / frameDelay.count()).count();
        updateCtx.ElapsedTime = elapsedTime;
        updateCtx.TotalTime += elapsedTime;
        OnUpdate.Call(updateCtx);

        frameElap += elapsedTime;
        if (frameDelay <= frameElap)
        {
            frameElap = 0ms;
            ++frames;

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            OnRender.Call(renderCtx);

            glfwSwapBuffers(_glfwWindow);
        }

        fpsElap += elapsedTime;
        if (fpsDelay <= fpsElap)
        {
            updateCtx.CurrentFPS = (frames / fpsElap.count()) * 1000.0;

            frames = 0;
            fpsElap = 0ms;
        }
    }

    OnStop.Call();

    glfwHideWindow(_glfwWindow);
}

void App::Stop()
{
    _running = false;
}

void App::Serialize(nlohmann::json& data)
{
    data["Size"] = { _windowSize.x, _windowSize.y };
    data["Title"] = _windowTitle;
}

void App::Deserialize(nlohmann::json& data)
{
    if (data.find("Size") != data.end())
    {
        _windowSize.x = data["Size"][0];
        _windowSize.x = data["Size"][0];
    }

	if (data.find("Title") != data.end())
	{
		_windowTitle = data["Title"].get<std::string>();
	}
}

bool App::LoadConfig(const std::string& filename)
{
    DuskBenchStart();

    std::ifstream file(filename);
    nlohmann::json data;

    DuskLogLoad("Loading config file '%s'", filename.c_str());

    if (!file.is_open())
    {
        DuskLogError("Failed to open config file '%s'", filename.c_str());
        return false;
    }

	data << file;
    Deserialize(data);

    file.close();

    DuskBenchEnd("App::LoadConfig()");
    return true;
}

bool App::SaveConfig(const std::string& filename)
{
    DuskBenchStart();

    std::ofstream file(filename);
    nlohmann::json data;

    DuskLogLoad("Saving config file '%s'", filename.c_str());

    if (!file.is_open())
    {
        DuskLogError("Failed to open config file '%s'", filename.c_str());
        return false;
    }

    Serialize(data);
    file << data;

    file.close();

    DuskBenchEnd("App::SaveConfig()");
    return true;
}

void App::SetWindowSize(const glm::ivec2& size)
{
    _windowSize = size;
    glfwSetWindowSize(_glfwWindow, _windowSize.x, _windowSize.y);
}

void App::SetWindowTitle(const std::string& title)
{
    _windowTitle = title;
    glfwSetWindowTitle(_glfwWindow, _windowTitle.c_str());
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

    _glfwWindow = glfwCreateWindow(_windowSize.x, _windowSize.y, _windowTitle.c_str(), NULL, NULL);
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
    DuskLogVerbose("Running %dx AA", samples);

    ShaderProgram::InitializeUniformBuffers();

    ImGui_ImplGlfwGL3_Init(_glfwWindow, false);

    glfwSetKeyCallback(_glfwWindow, &App::GLFW_KeyCallback);
    glfwSetMouseButtonCallback(_glfwWindow, &App::GLFW_MouseButtonCallback);
    glfwSetCursorPosCallback(_glfwWindow, &App::GLFW_MouseMoveCallback);
    glfwSetScrollCallback(_glfwWindow, &App::GLFW_ScrollCallback);
    glfwSetCharCallback(_glfwWindow, &App::GLFW_CharCallback);
    glfwSetDropCallback(_glfwWindow, &App::GLFW_DropCallback);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    // TODO: Move
    //_shaders.emplace("_default_text", std::unique_ptr<Shader>(new Shader({
    //    { GL_VERTEX_SHADER,   "assets/shaders/default/text.vs.glsl" },
    //    { GL_FRAGMENT_SHADER, "assets/shaders/default/text.fs.glsl"}
    //})));
    //_defaultFont = std::shared_ptr<Font>(new Font("assets/fonts/default.ttf", 36));

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

void App::GLFW_ErrorCallback(int code, const char * message)
{
    DuskLogError("GLFW: %d, %s", code, message);
}

void App::GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
    _KeyFunc(key, scancode, action, mods);
}

void App::GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
    _MouseButtonFunc(button, action, mods);
}

void App::GLFW_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
    _ScrollFunc(xoffset, yoffset);
}

void App::GLFW_CharCallback(GLFWwindow* window, unsigned int c)
{
    ImGui_ImplGlfwGL3_CharCallback(window, c);
}

void App::GLFW_MouseMoveCallback(GLFWwindow* window, double x, double y)
{
    _MouseMoveFunc(x, y);
}

void App::GLFW_DropCallback(GLFWwindow* window, int count, const char ** filenames)
{
    _DropFunc(count, filenames);
}

} // namespace dusk
