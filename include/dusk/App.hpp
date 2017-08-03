#ifndef DUSK_APP_HPP
#define DUSK_APP_HPP

#include <dusk/Config.hpp>
#include <dusk/Event.hpp>

#include <string>
#include <memory>
#include <chrono>

namespace dusk {

typedef int Key;
typedef int Button;
typedef unsigned int Flags;

struct UpdateContext
{
    float TargetFPS;
    float CurrentFPS;

    float DeltaTime;

    std::chrono::duration<double, std::milli> ElapsedTime;
    std::chrono::duration<double, std::milli> TotalTime;
};

struct RenderContext
{

};

class App
{
public:

    /// Class Boilerplate

    DISALLOW_COPY_AND_ASSIGN(App);

    explicit App(int argc, char** argv);
    virtual ~App();

    void Serialize(nlohmann::json& data);
    void Deserialize(nlohmann::json& data);

    /// Methods

    void Start();
    void Stop();

    bool LoadConfig(const std::string& filename);
    bool SaveConfig(const std::string& filename);

    glm::ivec2 GetWindowSize() const { return _windowSize; }
    void SetWindowSize(const glm::ivec2& size);

    std::string GetWindowTitle() const { return _windowTitle; }
    void SetWindowTitle(const std::string& title);

    /// Events

    Event<> OnStart;
    Event<> OnStop;

    Event<UpdateContext> OnUpdate;
    Event<RenderContext> OnRender;

    Event<Key, Flags> OnKeyPress;
    Event<Key, Flags> OnKeyRelease;

    Event<Button, Flags> OnMousePress;
    Event<Button, Flags> OnMouseRelease;
    Event<glm::vec2, glm::vec2> OnMouseMove;
    Event<glm::vec2> OnMouseScroll;

    Event<glm::ivec2> OnWindowResize;

    Event<std::vector<std::string>> OnFileDrop;

    // Assets

private:

    void CreateWindow();
    void DestroyWindow();

    bool _running;

    glm::ivec2 _windowSize = { 640, 480 };
    std::string _windowTitle = "Dusk";
    float _targetFps = 60.0f;

    ALCdevice * _alDevice;
    ALCcontext * _alContext;

    GLFWwindow * _glfwWindow;

    /// GLFW Accessor Methods

    static void GLFW_ErrorCallback(int code, const char * message);
    static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GLFW_MouseMoveCallback(GLFWwindow* window, double x, double y);
    static void GLFW_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void GLFW_CharCallback(GLFWwindow* window, unsigned int c);
    static void GLFW_DropCallback(GLFWwindow* window, int count, const char ** filenames);

    static std::function<void(int, int, int, int)> _KeyFunc;
    static std::function<void(int, int, int)>      _MouseButtonFunc;
    static std::function<void(double, double)>     _MouseMoveFunc;
    static std::function<void(double, double)>     _ScrollFunc;
    static std::function<void(unsigned int)>       _CharFunc;
    static std::function<void(int, const char **)> _DropFunc;

}; // class App

} // namespace dusk

#endif // DUSK_APP_HPP
