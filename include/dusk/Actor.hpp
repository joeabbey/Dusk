#ifndef DUSK_ACTOR_HPP
#define DUSK_ACTOR_HPP

#include <dusk/Config.hpp>
#include <dusk/EventDispatcher.hpp>
#include <dusk/Component.hpp>
//#include <dusk/MeshComponent.hpp>
//#include <dusk/ScriptComponent.hpp>

#include <typeindex>
#include <typeinfo>

namespace dusk {

class Scene;

class Actor : public IEventDispatcher
{
public:

    enum class Events : EventID
    {
        _PREFIX = 200,
        UPDATE,
        RENDER,
    };

    DISALLOW_COPY_AND_ASSIGN(Actor);

    Actor(Scene * parent);
    virtual ~Actor();

    static std::unique_ptr<Actor> Parse(nlohmann::json & data, Scene * parent);

    Scene * GetScene() const { return _parent; };

    void SetBaseTransform(const glm::mat4& baseTransform);

    void SetPosition(const glm::vec3& pos);
    glm::vec3 GetPosition() const { return _position; }

    void SetRotation(const glm::vec3& rot);
    glm::vec3 GetRotation() const { return _rotation; }

    void SetScale(const glm::vec3& scale);
    glm::vec3 GetScale() const { return _scale; }

    glm::mat4 GetTransform();

    void AddComponent(std::unique_ptr<Component> comp);

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

    Scene * _parent;

    std::string _name;

    glm::mat4 _baseTransform;
    glm::mat4 _transform;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    std::vector<std::unique_ptr<Component>> _components;

}; // class Actor

} // namespace dusk

#endif // DUSK_ACTOR_HPP
