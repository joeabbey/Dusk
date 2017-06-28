#include "dusk/Actor.hpp"

#include <dusk/Benchmark.hpp>
#include <dusk/Scene.hpp>

namespace dusk {

Actor::Actor(bool isTempalte /*= false*/)
    : _scene(nullptr)
    , _isTemplate(isTempalte)
    , _baseTransform(1)
    , _transform(1)
    , _position(0)
    , _rotation(0)
    , _scale(1)
{
}

std::unique_ptr<Actor> Actor::Parse(nlohmann::json & data)
{
    bool isTemplate = false;
    if (data.find("Template") != data.end())
    {
        isTemplate = data["Template"];
    }

    Actor * actor = new Actor(isTemplate);

	if (data.find("Position") != data.end())
	{
		actor->SetPosition({
			data["Position"][0], data["Position"][1], data["Position"][2]
		});
	}

	if (data.find("Rotation") != data.end())
	{
		actor->SetRotation({
			data["Rotation"][0], data["Rotation"][1], data["Rotation"][2]
		});
	}

	if (data.find("Scale") != data.end())
	{
		actor->SetScale({
			data["Scale"][0], data["Scale"][1], data["Scale"][2]
		});
	}

	for (auto& component : data["Components"])
	{
        actor->AddComponent(Component::Parse(component, isTemplate));
	}

    return std::unique_ptr<Actor>(actor);
}

std::unique_ptr<Actor> Actor::Clone()
{
    std::unique_ptr<Actor> actor(new Actor());

    actor->SetPosition(GetPosition());
    actor->SetRotation(GetRotation());
    actor->SetScale(GetScale());

    for (auto& componenet : _components)
    {
        actor->AddComponent(componenet->Clone());
    }

    actor->SetScene(GetScene());

    return actor;
}

Actor::~Actor()
{
    if (GetScene() && !IsTemplate())
    {
        GetScene()->RemoveEventListener((EventID)Scene::Events::UPDATE, this, &Actor::Update);
        GetScene()->RemoveEventListener((EventID)Scene::Events::RENDER, this, &Actor::Render);
    }
}

void Actor::SetScene(Scene * scene)
{
    if (GetScene() && !IsTemplate())
    {
        GetScene()->RemoveEventListener((EventID)Scene::Events::UPDATE, this, &Actor::Update);
        GetScene()->RemoveEventListener((EventID)Scene::Events::RENDER, this, &Actor::Render);
    }

    _scene = scene;

    if (!IsTemplate())
    {
        GetScene()->AddEventListener((EventID)Scene::Events::UPDATE, this, &Actor::Update);
        GetScene()->AddEventListener((EventID)Scene::Events::RENDER, this, &Actor::Render);
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

void Actor::Update(const Event& event)
{
    DispatchEvent(Event((EventID)Events::UPDATE, event.GetData()));
}

void Actor::Render(const Event& event)
{
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
