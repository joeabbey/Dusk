#ifndef DUSK_ACTOR_HPP
#define DUSK_ACTOR_HPP

#include <dusk/Config.hpp>
#include <dusk/EventDispatcher.hpp>
#include <dusk/Component.hpp>
//#include <dusk/MeshComponent.hpp>
//#include <dusk/ScriptComponent.hpp>

namespace dusk {

class Scene;

class Actor : public IEventDispatcher
{
public:

    enum class Events : EventID
    {
        _PREFIX = 200,
        LOAD_START,
        FREE_START,
        LOAD_FINISHED,
        FREE_FINISHED,
        UPDATE,
        RENDER,
    };

    DISALLOW_COPY_AND_ASSIGN(Actor);

    Actor(Scene * parent, const std::string& name);
    virtual ~Actor();

    bool IsLoaded() const { return _loaded; }

    Scene * GetScene() const { return _parent; };

    std::string GetName() const { return _name; }

    void SetBaseTransform(const glm::mat4& baseTransform);

    void SetPosition(const glm::vec3& pos);
    glm::vec3 GetPosition() const { return _position; }

    void SetRotation(const glm::vec3& rot);
    glm::vec3 GetRotation() const { return _rotation; }

    void SetScale(const glm::vec3& scale);
    glm::vec3 GetScale() const { return _scale; }

    glm::mat4 GetTransform();

    void AddComponent(Component * comp);

    std::vector<Component *> GetComponents() { return _components; }
    std::vector<Component *> GetComponentsByTag(const std::string& tag) { return _componentsByTag[tag]; }
    Component * GetComponentByName(const std::string& name) { return _componentsByName[name]; }

    virtual bool Load();
    virtual void Free();

    virtual void Update(const Event& event);
    virtual void Render(const Event& event);

    static void InitScripting();

    static int Script_GetPosition(lua_State * L);
    static int Script_SetPosition(lua_State * L);
    static int Script_GetRotation(lua_State * L);
    static int Script_SetRotation(lua_State * L);
    static int Script_GetScale(lua_State * L);
    static int Script_SetScale(lua_State * L);

private:

    bool _loaded = false;

    Scene * _parent;

    std::string _name;

    glm::mat4 _baseTransform;
    glm::mat4 _transform;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    std::vector<Component*> _components;

    std::unordered_map<std::string, Component*> _componentsByName;
    std::unordered_map<std::string, std::vector<Component*>> _componentsByTag;

}; // class Actor

} // namespace dusk

#endif // DUSK_ACTOR_HPP
