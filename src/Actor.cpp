#include "dusk/Actor.hpp"

#include <dusk/Benchmark.hpp>

namespace dusk {

Actor::Actor(const std::string& name)
    : _loaded(false)
    , _name(name)
    , _components()
{

}

Actor::~Actor()
{
    for (Component * comp : _components)
    {
        delete comp;
    }
}

void Actor::SetPosition(glm::vec3 pos)
{
    _position = pos;
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

void Actor::Update()
{
    for (Component * comp : _components)
    {
        comp->Update();
    }
}

void Actor::Render()
{
    for (Component * comp : _components)
    {
        comp->Render();
    }
}


} // namespace dusk
