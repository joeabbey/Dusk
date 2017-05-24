#ifndef DUSK_ACTOR_HPP
#define DUSK_ACTOR_HPP

#include <dusk/Config.hpp>
#include <dusk/Component.hpp>
//#include <dusk/MeshComponent.hpp>
//#include <dusk/ScriptComponent.hpp>

namespace dusk {

class Scene;

class Actor
{
public:

    DISALLOW_COPY_AND_ASSIGN(Actor);

    Actor(Scene * parent, const std::string& name);
    virtual ~Actor();

    bool IsLoaded() const { return _loaded; }

    Scene * GetScene() const { return _parent; };

    std::string GetName() const { return _name; }

    void SetPosition(glm::vec3 pos);
    glm::vec3 GetPosition() const { return _position; }

    void SetRotation(glm::vec3 rot);
    glm::vec3 GetRotation() const { return _rotation; }

    void SetScale(glm::vec3 scale);
    glm::vec3 GetScale() const { return _scale; }

    glm::mat4 GetTransform();

    void AddComponent(Component * comp);

    std::vector<Component *> GetComponents() { return _components; }
    std::vector<Component *> GetComponentsByTag(const std::string& tag) { return _componentsByTag[tag]; }
    Component * GetComponentByName(const std::string& name) { return _componentsByName[name]; }

    bool Load();
    void Free();

    void Update();
    void Render();

private:

    bool _loaded;

    Scene * _parent;

    std::string _name;
    glm::vec3   _position;
    glm::vec3   _rotation;
    glm::vec3   _scale;

    bool _transformInvalid = true;
    glm::mat4   _transform;

    std::vector<Component*> _components;

    std::unordered_map<std::string, Component*> _componentsByName;
    std::unordered_map<std::string, std::vector<Component*>> _componentsByTag;

}; // class Actor

} // namespace dusk

#endif // DUSK_ACTOR_HPP
