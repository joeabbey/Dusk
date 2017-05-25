#include "dusk/Actor.hpp"

#include <dusk/Benchmark.hpp>
#include <dusk/Scene.hpp>

namespace dusk {

Actor::Actor(Scene * parent, const std::string& name)
    : _loaded(false)
    , _parent(parent)
    , _name(name)
    , _position(0)
    , _rotation(0)
    , _scale(1)
    , _components()
{
    GetScene()->AddEventListener((EventID)Scene::Events::UPDATE, this, &Actor::Update);
    GetScene()->AddEventListener((EventID)Scene::Events::RENDER, this, &Actor::Render);
}

Actor::~Actor()
{
    GetScene()->RemoveEventListener((EventID)Scene::Events::UPDATE, this, &Actor::Update);
    GetScene()->RemoveEventListener((EventID)Scene::Events::RENDER, this, &Actor::Render);

    for (Component * comp : _components)
    {
        delete comp;
    }
}

void Actor::SetPosition(glm::vec3 pos)
{
    _position = pos;
}

void Actor::SetRotation(glm::vec3 rot)
{
    _rotation = rot;
}

void Actor::SetScale(glm::vec3 scale)
{
    _scale = scale;
}

glm::mat4 Actor::GetTransform()
{
    _transform = glm::mat4(1);
    _transform = glm::scale(_transform, _scale);
    _transform = glm::rotate(_transform, _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    _transform = glm::translate(_transform, _position);

    return _transform;
}

void Actor::AddComponent(Component * comp)
{
    _components.push_back(comp);
}

bool Actor::Load()
{
    DuskBenchStart();

    for (Component * comp : _components)
    {
        if (!comp->Load())
        {
            DuskLogError("Failed to load actor '%s'", GetName().c_str());
            return false;
        }
    }

    _loaded = true;

    DuskBenchEnd("Actor::Load()");
    return true;
}

void Actor::Free()
{
    for (Component * comp : _components)
    {
        comp->Free();
    }
}

void Actor::Update(const Event& event)
{
    if (!_loaded) return;

    DispatchEvent(Event((EventID)Events::UPDATE));
}

void Actor::Render(const Event& event)
{
    if (!_loaded) return;

    DispatchEvent(Event((EventID)Events::RENDER));
}


} // namespace dusk
