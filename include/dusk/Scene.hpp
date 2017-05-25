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
        UPDATE,
        RENDER,
    };

    DISALLOW_COPY_AND_ASSIGN(Scene);

    Scene(const std::string& name);
    virtual ~Scene();

    bool IsLoaded() const { return _loaded; }

    std::string GetName() const { return _name; }

    void AddActor(Actor * actor);
    const std::vector<Actor *>& GetActors() const { return _actors; };

    void AddCamera(Camera * camera);
    const std::vector<Camera *>& GetCameras() const { return _cameras; };

    void SetCamera(Camera * camera) { _camera = camera; }
    Camera * GetCamera() const { return _camera; };

    bool Load();
    void Free();

    void Update();
    void Render();

private:

    bool _loaded;

    std::string _name;

    Camera * _camera;

    std::vector<Camera *> _cameras;
    std::vector<Actor *> _actors;

}; // class Scene

} // namespace dusk

#endif // DUSK_SCENE_HPP
