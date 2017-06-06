#include "dusk/Actor.hpp"

#include <dusk/Benchmark.hpp>
#include <dusk/Scene.hpp>

namespace dusk {

Actor::Actor(Scene * parent, const std::string& name)
    : _loaded(false)
    , _parent(parent)
    , _name(name)
    , _baseTransform(1)
    , _transform(1)
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

    DispatchEvent(Event((EventID)Events::UPDATE, event.GetData()));
}

void Actor::Render(const Event& event)
{
    if (!_loaded) return;

    //DuskLogInfo("Rendering %s", GetName().c_str());

    DispatchEvent(Event((EventID)Events::RENDER));
}

void Actor::InitScripting()
{
    ScriptHost::AddFunction("dusk_Actor_GetPosition", &Actor::Script_GetPosition);
    ScriptHost::AddFunction("dusk_Actor_SetPosition", &Actor::Script_SetPosition);
    ScriptHost::AddFunction("dusk_Actor_GetRotation", &Actor::Script_GetRotation);
    ScriptHost::AddFunction("dusk_Actor_SetRotation", &Actor::Script_SetRotation);
    ScriptHost::AddFunction("dusk_Actor_GetScale", &Actor::Script_GetScale);
    ScriptHost::AddFunction("dusk_Actor_SetScale", &Actor::Script_SetScale);
}

int Actor::Script_GetPosition(lua_State * L)
{
    Actor * actor = (Actor *)lua_tointeger(L, 1);
    glm::vec3 pos = actor->GetPosition();

    lua_pushnumber(L, pos.x);
    lua_pushnumber(L, pos.y);
    lua_pushnumber(L, pos.z);

    return 3;
}

int Actor::Script_SetPosition(lua_State * L)
{
    Actor * actor = (Actor *)lua_tointeger(L, 1);
    glm::vec3 pos;

    pos.x = (float)lua_tonumber(L, 2);
    pos.y = (float)lua_tonumber(L, 3);
    pos.z = (float)lua_tonumber(L, 4);

    actor->SetPosition(pos);

    return 0;
}

int Actor::Script_GetRotation(lua_State * L)
{
    Actor * actor = (Actor *)lua_tointeger(L, 1);
    glm::vec3 rot = actor->GetRotation();

    lua_pushnumber(L, rot.x);
    lua_pushnumber(L, rot.y);
    lua_pushnumber(L, rot.z);

    return 3;
}

int Actor::Script_SetRotation(lua_State * L)
{
    Actor * actor = (Actor *)lua_tointeger(L, 1);
    glm::vec3 rot;

    rot.x = (float)lua_tonumber(L, 2);
	rot.y = (float)lua_tonumber(L, 3);
	rot.z = (float)lua_tonumber(L, 4);

    actor->SetRotation(rot);

    return 0;
}

int Actor::Script_GetScale(lua_State * L)
{
    Actor * actor = (Actor *)lua_tointeger(L, 1);
    glm::vec3 scale = actor->GetScale();

    lua_pushnumber(L, scale.x);
    lua_pushnumber(L, scale.y);
    lua_pushnumber(L, scale.z);

    return 3;
}

int Actor::Script_SetScale(lua_State * L)
{
    Actor * actor = (Actor *)lua_tointeger(L, 1);
    glm::vec3 scale;

    scale.x = (float)lua_tonumber(L, 2);
	scale.y = (float)lua_tonumber(L, 3);
	scale.z = (float)lua_tonumber(L, 4);

    actor->SetScale(scale);

    return 0;
}


} // namespace dusk
