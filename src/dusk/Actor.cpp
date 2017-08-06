#include "dusk/Actor.hpp"

#include <dusk/Benchmark.hpp>
#include <dusk/Scene.hpp>

namespace dusk {

Actor::Actor()
    : _scene(nullptr)
    , _baseTransform(1)
    , _transform(1)
    , _position(0)
    , _rotation(0)
    , _scale(1)
{
}

void Actor::SetScene(Scene * scene)
{
    _scene = scene;
}

void Actor::SetBaseTransform(const glm::mat4& baseTransform)
{
    _baseTransform = baseTransform;
}

void Actor::SetPosition(const glm::vec3& pos)
{
    _position = pos;
}

void Actor::SetRotation(const glm::vec3& rot)
{
    _rotation = rot;
}

void Actor::SetScale(const glm::vec3& scale)
{
    _scale = scale;
}

glm::mat4 Actor::GetTransform()
{
    _transform = _baseTransform;
    _transform = glm::translate(_transform, _position);
    _transform = glm::rotate(_transform, _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    _transform = glm::scale(_transform, _scale);

    return _transform;
}

void Actor::AddComponent(std::unique_ptr<Component> comp)
{
    comp->SetActor(this);
    _components.push_back(std::move(comp));
}

void Actor::Update(const UpdateContext& ctx)
{
    EvtUpdate.Call(ctx);
}

void Actor::Render(RenderContext& ctx)
{
    EvtRender.Call(ctx);
}

} // namespace dusk
