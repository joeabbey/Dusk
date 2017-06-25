#ifndef DUSK_APP_HPP
#define DUSK_APP_HPP

#include <dusk/Config.hpp>
#include <dusk/EventDispatcher.hpp>
#include <dusk/Scene.hpp>
#include <dusk/Asset.hpp>

#include <string>
#include <stack>
#include <unordered_map>
#include <memory>

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
        START,
        STOP,
    };

    DISALLOW_COPY_AND_ASSIGN(App);

    static App * GetInst() { return _Inst; }

    explicit App(int argc, char** argv);
    virtual ~App();

    void LoadConfig(const std::string& filename);

    Shader * GetShader(const std::string& name) const { return _shaders.at(name).get(); }

    Scene * GetScene() const { return _scene.get(); };

    void Run();

    // TODO: Make private
    unsigned int WindowWidth  = 1024;
    unsigned int WindowHeight = 768;
    std::string WindowTitle   = "Dusk";

    static void InitScripting();
    static int Script_GetInst(lua_State * L);
    static int Script_LoadConfig(lua_State * L);
    static int Script_GetScene(lua_State * L);

    AssetCache<Texture> * GetTextureCache() const { return _textureCache.get(); }
    AssetIndex<Texture> * GetTextureIndex() const { return _textureIndex.get(); }
    AssetCache<Mesh> * GetMeshCache() const { return _meshCache.get(); }
    AssetIndex<Mesh> * GetMeshIndex() const { return _meshIndex.get(); }

    GLFWwindow * GetGLFWWindow() const { return _glfwWindow; }

    static void GLFW_ErrorCallback(int code, const char * message);
    static void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GLFW_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GLFW_CharCallback(GLFWwindow* window, unsigned int c);

private:

    static App * _Inst;

	bool ParseWindow(nlohmann::json& data);

    void CreateWindow();
    void DestroyWindow();

    const float TARGET_FPS = 60.0f;

    std::unique_ptr<AssetCache<Texture>> _textureCache;
    std::unique_ptr<AssetIndex<Texture>> _textureIndex;
    std::unique_ptr<AssetCache<Mesh>> _meshCache;
    std::unique_ptr<AssetIndex<Mesh>> _meshIndex;

    std::unordered_map<std::string, std::unique_ptr<Shader>> _shaders;

    std::unique_ptr<Scene> _scene;

    ALCdevice * _alDevice;
    ALCcontext * _alContext;

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
