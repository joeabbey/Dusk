#ifndef DUSK_ACTOR_HPP
#define DUSK_ACTOR_HPP

#include <dusk/Config.hpp>
#include <dusk/Component.hpp>
//#include <dusk/MeshComponent.hpp>
//#include <dusk/ScriptComponent.hpp>

namespace dusk {

class Actor
{
public:

    Actor(const std::string& name);
    virtual ~Actor();

    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;

    bool IsLoaded() const { return _loaded; }

    std::string GetName() const { return _name; }

    void SetPosition(glm::vec3 pos);
    glm::vec3 GetPosition() const { return _position; }

    void AddComponent(Component * comp);
    const std::vector<Component*> GetComponents() const { return _components; }

    bool Load();
    void Free();

    void Update();
    void Render();

private:

    bool _loaded;

    std::string _name;
    glm::vec3   _position;

    std::vector<Component*> _components;

}; // class Actor

} // namespace dusk

#endif // DUSK_ACTOR_HPP
