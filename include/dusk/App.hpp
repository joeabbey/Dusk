#ifndef DUSK_APP_HPP
#define DUSK_APP_HPP

#include <dusk/Config.hpp>
#include <dusk/EventDispatcher.hpp>
#include <dusk/Scene.hpp>

#include <string>
#include <stack>
#include <unordered_map>

namespace dusk {

class UpdateEventData;

class App : public IEventDispatcher
{
public:

    enum class Events : EventID
    {
        _PREFIX = 0,
        UPDATE,
        RENDER,
    };

    DISALLOW_COPY_AND_ASSIGN(App);

    static App * GetInst() { return _Inst; }

    explicit App(int argc, char** argv);
    virtual ~App();

    bool LoadConfig(const std::string& filename);

    Scene * GetScene() const { return (_sceneStack.empty() ? nullptr : _sceneStack.top()); };
    Scene * GetSceneByName(const std::string& name) const;
    void PushScene(Scene * scene);
    void PopScene();

    void Run();

    // TODO: Make private
    unsigned int WindowWidth  = 1024;
    unsigned int WindowHeight = 768;
    std::string WindowTitle   = "Dusk";

    static void InitScripting();
    static int Script_GetInst(lua_State * L);
    static int Script_LoadConfig(lua_State * L);
    static int Script_GetScene(lua_State * L);
    static int Script_GetSceneByName(lua_State * L);
    static int Script_PushScene(lua_State * L);
    static int Script_PopScene(lua_State * L);

    static void GLFW_ErrorCallback(int code, const char * message);
    static void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GLFW_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GLFW_CharCallback(GLFWwindow* window, unsigned int c);

private:

    static App * _Inst;

	bool ParseWindow(nlohmann::json& data);
	bool ParseShader(nlohmann::json& data);
	bool ParseScene(nlohmann::json & data);
	Camera * ParseCamera(nlohmann::json& data);
	Mesh * ParseMesh(nlohmann::json& data);
	Actor * ParseActor(nlohmann::json& data, Scene * scene);
	Component * ParseComponent(nlohmann::json& data, Actor * actor);

    void CreateWindow();
    void DestroyWindow();

    const float TARGET_FPS = 60.0f;

    // TODO: Move
    std::unordered_map<std::string, Shader *> _shaders;

    Scene * _startScene = nullptr;
    std::stack<Scene *> _sceneStack;
    std::unordered_map<std::string, Scene *> _scenes;

    GLFWwindow * _glfwWindow;

}; // class App

class UpdateEventData : public EventData
{
public:

    UpdateEventData() = default;
    UpdateEventData(const UpdateEventData&) = default;
    virtual ~UpdateEventData() = default;
    UpdateEventData& operator=(const UpdateEventData&) = default;

    float GetDelta() const { return _delta; }
    double GetElapsedTime() const { return _elapsed_time; }
    double GetTotalTime() const { return _total_time; }
    float GetCurrentFPS() const { return _current_fps; }

    virtual int PushToLua(lua_State * L) const override;

    void SetTargetFPS(float fps) { _target_fps = fps; }
    void SetCurrentFPS(float fps) { _current_fps = fps; }

    void Update(double elapsed);

private:

    float  _delta;
    double _elapsed_time;
    double _total_time;

    float _target_fps = 60.0f;
    float  _current_fps;

}; // class UpdateEventData

} // namespace dusk

#endif // DUSK_APP_HPP
