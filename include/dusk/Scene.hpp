#ifndef DUSK_SCENE_HPP
#define DUSK_SCENE_HPP

#include <dusk/Config.hpp>

#include <dusk/EventDispatcher.hpp>
#include <dusk/Actor.hpp>
#include <dusk/Camera.hpp>
#include <string>
#include <vector>
#include <memory>

namespace dusk {

class Scene : public IEventDispatcher
{
public:

    enum class Events : EventID
    {
        _PREFIX = 100,
        START,
        STOP,
        UPDATE,
        RENDER,
    };

    DISALLOW_COPY_AND_ASSIGN(Scene);

    Scene();
    virtual ~Scene();

    static std::unique_ptr<Scene> Parse(nlohmann::json & data);

    void RunScript(const std::string& filename) { _scriptHost.RunFile(filename); }

    void AddActor(std::unique_ptr<Actor> actor);
    void AddActorTemplate(const std::string& id, std::unique_ptr<Actor> actor);

    void AddCamera(std::unique_ptr<Camera> camera);

    Actor * GetActorTemplate(const std::string& id);

    void SetCurrentCamera(Camera * camera) { _currentCamera = camera; }
    Camera * GetCurrentCamera() const { return _currentCamera; };

    void Start();
    void Stop();

    void Update(const Event& event);
    void Render(const Event& event);

    static void InitScripting();
    static int Script_GetActorByName(lua_State * L);

private:

    Camera * _currentCamera;

    ScriptHost _scriptHost;

    std::vector<std::unique_ptr<Camera>> _cameras;
    std::vector<std::unique_ptr<Actor>> _actors;

    std::unordered_map<std::string, std::unique_ptr<Actor>> _actorTemplates;

}; // class Scene

} // namespace dusk

#endif // DUSK_SCENE_HPP
