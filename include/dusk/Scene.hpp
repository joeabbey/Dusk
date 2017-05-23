#ifndef DUSK_SCENE_HPP
#define DUSK_SCENE_HPP

#include <dusk/Config.hpp>
#include <dusk/Actor.hpp>

#include <string>
#include <vector>

namespace dusk {

class Scene
{
public:

    Scene(const std::string& name);
    virtual ~Scene();

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    bool IsLoaded() const { return _loaded; }

    std::string GetName() const { return _name; }

    void AddActor(Actor * actor);
    const std::vector<Actor*>& GetActors() const { return _actors; };

    bool Load();
    void Free();

    void Update();
    void Render();

private:

    bool _loaded;

    std::string _name;

    std::vector<Actor*> _actors;

}; // class Scene

} // namespace dusk

#endif // DUSK_SCENE_HPP
