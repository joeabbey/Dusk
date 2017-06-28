#include "dusk/Component.hpp"

#include <dusk/Log.hpp>
#include <dusk/App.hpp>
#include <dusk/Actor.hpp>

namespace dusk {

Component::Component(bool isTempalte /*= false*/)
    : _actor(nullptr)
    , _isTemplate(isTempalte)
{
}

Component::~Component()
{
}

std::unique_ptr<Component> Component::Parse(nlohmann::json & data, bool isTemplate /*= false*/)
{
    std::unique_ptr<Component> component;

	const std::string& type = data["Type"];
	if ("Model" == type)
	{
		component.reset(new ModelComponent(Model::Parse(data), isTemplate));
	}
	else if ("Camera" == type)
	{
		component.reset(new CameraComponent(Camera::Parse(data), isTemplate));
	}
	else if ("Script" == type)
	{
		component.reset(new ScriptComponent(data["File"].get<std::string>(), isTemplate));
	}

	return component;
}

std::unique_ptr<Component> Component::Clone()
{
    Component * component = new Component();

    component->SetActor(GetActor());

    return std::unique_ptr<Component>(component);
}

void Component::SetActor(Actor * actor)
{
    _actor = actor;
}

void Component::InitScripting()
{
    ScriptHost::AddFunction("dusk_Component_GetActor", &Component::Script_GetActor);

    //ModelComponent::InitScripting();
    //CameraComponent::InitScripting();
    //ScriptComponent::InitScripting();
}

int Component::Script_GetActor(lua_State * L)
{
    Component * componenet = (Component *)lua_tointeger(L, 1);

    lua_pushinteger(L, (ptrdiff_t)componenet->GetActor());

    return 1;
}

ModelComponent::ModelComponent(std::unique_ptr<Model> model, bool isTempalte /*= false*/)
    : Component(isTempalte)
    , _model(std::move(model))
{
}

ModelComponent::~ModelComponent()
{
    if (GetActor() && !IsTemplate())
    {
        GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &ModelComponent::OnUpdate);
        GetActor()->RemoveEventListener((EventID)Actor::Events::RENDER, this, &ModelComponent::OnRender);
    }
}

std::unique_ptr<Component> ModelComponent::Clone()
{
    ModelComponent * component = new ModelComponent(GetModel()->Clone());

    component->SetActor(GetActor());

    return std::unique_ptr<Component>(component);
}

void ModelComponent::SetActor(Actor * actor)
{
    if (GetActor() && !IsTemplate())
    {
        GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &ModelComponent::OnUpdate);
        GetActor()->RemoveEventListener((EventID)Actor::Events::RENDER, this, &ModelComponent::OnRender);
    }

    Component::SetActor(actor);

    if (!IsTemplate())
    {
        GetActor()->AddEventListener((EventID)Actor::Events::UPDATE, this, &ModelComponent::OnUpdate);
        GetActor()->AddEventListener((EventID)Actor::Events::RENDER, this, &ModelComponent::OnRender);
    }
}

void ModelComponent::OnUpdate(const Event& event)
{
    _model->SetBaseTransform(GetActor()->GetTransform());
    _model->Update();
}

void ModelComponent::OnRender(const Event& event)
{
    _model->Render();
}

CameraComponent::CameraComponent(std::unique_ptr<Camera> camera, bool isTempalte /*= false*/)
    : Component(isTempalte)
    , _camera(std::move(camera))
{
}

CameraComponent::~CameraComponent()
{
    if (GetActor() && !IsTemplate())
    {
        GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &CameraComponent::OnUpdate);
    }
}

std::unique_ptr<Component> CameraComponent::Clone()
{
    CameraComponent * component = new CameraComponent(GetCamera()->Clone());

    component->SetActor(GetActor());

    return std::unique_ptr<Component>(component);
}

void CameraComponent::SetActor(Actor * actor)
{
    if (GetActor() && !IsTemplate())
    {
        GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &CameraComponent::OnUpdate);
    }

    Component::SetActor(actor);

    if (!IsTemplate())
    {
        GetActor()->AddEventListener((EventID)Actor::Events::UPDATE, this, &CameraComponent::OnUpdate);
    }
}

void CameraComponent::OnUpdate(const Event& event)
{
    _camera->SetBaseTransform(glm::inverse(GetActor()->GetTransform()));
}

ScriptComponent::ScriptComponent(const std::string& filename, bool isTempalte /*= false*/)
    : Component(isTempalte)
    , _scriptHost()
    , _filename(filename)
{
    lua_State * L = _scriptHost.GetLuaState();

    lua_pushinteger(L, (ptrdiff_t)this);
    lua_setglobal(L, "dusk_current_ScriptComponent");

    // Wait until we have an actor to run the script
}

std::unique_ptr<Component> ScriptComponent::Clone()
{
    ScriptComponent * component = new ScriptComponent(_filename);

    component->SetActor(GetActor());

    return std::unique_ptr<Component>(component);
}

void ScriptComponent::SetActor(Actor * actor)
{
    if (GetActor() && !IsTemplate())
    {
        // TODO: Cleanup script host?
    }

    Component::SetActor(actor);

    if (!IsTemplate())
    {
        // TODO: Fix
        _scriptHost.RunFile(_filename);
    }
}

} // namespace dusk
