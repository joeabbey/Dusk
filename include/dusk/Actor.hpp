#ifndef DUSK_ACTOR_HPP
#define DUSK_ACTOR_HPP

#include <dusk/Config.hpp>

#include <dusk/ActorComponent.hpp>
#include <memory>

namespace dusk {

class Scene;

class Actor
{
public:

    DISALLOW_COPY_AND_ASSIGN(Actor);

    Actor();
    virtual ~Actor() = default;

    void SetScene(Scene * scene);
    Scene * GetScene() const { return _scene; };

    inline bool IsTemplate() const { return _isTemplate; }

    void SetBaseTransform(const glm::mat4& baseTransform);

    void SetPosition(const glm::vec3& pos);
    inline glm::vec3 GetPosition() const { return _position; }

    void SetRotation(const glm::vec3& rot);
    inline glm::vec3 GetRotation() const { return _rotation; }

    void SetScale(const glm::vec3& scale);
    inline glm::vec3 GetScale() const { return _scale; }

    glm::mat4 GetTransform();

    void AddComponent(std::unique_ptr<ActorComponent> comp);

    void Update(const UpdateContext& ctx);
    void Render(RenderContext& ctx);

    // OnStart ?
    // OnStop ?

    Event<const UpdateContext&> EvtUpdate;
    Event<RenderContext&> EvtRender;

private:

    Scene * _scene;

    glm::mat4 _baseTransform;
    glm::mat4 _transform;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    std::vector<std::unique_ptr<ActorComponent>> _components;

}; // class Actor

} // namespace dusk

#endif // DUSK_ACTOR_HPP
