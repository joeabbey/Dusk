#ifndef DUSK_SCENE_HPP
#define DUSK_SCENE_HPP

#include <dusk/Config.hpp>
#include <dusk/EventDispatcher.hpp>
#include <dusk/Actor.hpp>
#include <dusk/Camera.hpp>

#include <string>
#include <vector>

namespace dusk {

class Scene : public IEventDispatcher
{
public:

    enum class Events : EventID
    {
        _PREFIX = 100,
        LOAD,
        FREE,
        UPDATE,
        RENDER,
    };

    DISALLOW_COPY_AND_ASSIGN(Scene);

    Scene(const std::string& name);
    virtual ~Scene();

    bool IsLoaded() const { return _loaded; }

    std::string GetName() const { return _name; }

    void AddScript(const std::string& filename) { _scripts.push_back(filename); }

    void AddActor(Actor * actor);
    const std::vector<Actor *>& GetActors() const { return _actors; };
    Actor * GetActorByName(const std::string& name);

    void AddCamera(Camera * camera);
    const std::vector<Camera *>& GetCameras() const { return _cameras; };

    void SetCamera(Camera * camera) { _camera = camera; }
    Camera * GetCamera() const { return _camera; };

    bool Load();
    void Free();

    void Update(const Event& event);
    void Render(const Event& event);

    static void InitScripting();
    static int Script_GetActorByName(lua_State * L);

private:

    bool _loaded;

    std::string _name;

    Camera * _camera;

    ScriptHost _scriptHost;

    std::vector<std::string> _scripts;
    std::vector<Camera *> _cameras;
    std::vector<Actor *> _actors;

}; // class Scene

} // namespace dusk

#endif // DUSK_SCENE_HPP
