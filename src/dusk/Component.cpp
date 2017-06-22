#include "dusk/Component.hpp"

#include <dusk/Log.hpp>
#include <dusk/App.hpp>
#include <dusk/Actor.hpp>

namespace dusk {

Component::Component(Actor * parent)
    : _parent(parent)
{
}

Component::~Component()
{
}

std::unique_ptr<Component> Component::Parse(nlohmann::json & data, Actor * actor)
{
    std::unique_ptr<Component> component;

	const std::string& type = data["Type"];
	if ("Model" == type)
	{
		component.reset(new ModelComponent(actor, Model::Parse(data)));
	}
	else if ("Script" == type)
	{
		component.reset(new ScriptComponent(actor, data["File"].get<std::string>()));
	}
	else if ("Camera" == type)
	{
		component.reset(new CameraComponent(actor, Camera::Parse(data)));
	}

	return component;
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

ModelComponent::ModelComponent(Actor * parent, std::unique_ptr<Model> model)
    : Component(parent)
    , _model(std::move(model))
{
    GetActor()->AddEventListener((EventID)Actor::Events::UPDATE, this, &ModelComponent::Update);
    GetActor()->AddEventListener((EventID)Actor::Events::RENDER, this, &ModelComponent::Render);
}

ModelComponent::~ModelComponent()
{
    GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &ModelComponent::Update);
    GetActor()->RemoveEventListener((EventID)Actor::Events::RENDER, this, &ModelComponent::Render);
}

void ModelComponent::Update(const Event& event)
{
    _model->SetBaseTransform(GetActor()->GetTransform());
    _model->Update();
}

void ModelComponent::Render(const Event& event)
{
    _model->Render();
}

CameraComponent::CameraComponent(Actor * parent, std::unique_ptr<Camera> camera)
    : Component(parent)
    , _camera(std::move(camera))
{
    GetActor()->AddEventListener((EventID)Actor::Events::UPDATE, this, &CameraComponent::Update);
}

CameraComponent::~CameraComponent()
{
    GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &CameraComponent::Update);
}

void CameraComponent::Update(const Event& event)
{
    _camera->SetBaseTransform(glm::inverse(GetActor()->GetTransform()));
}

ScriptComponent::ScriptComponent(Actor * parent, const std::string& filename)
    : Component(parent)
    , _scriptHost()
    , _filename(filename)
{
    lua_State * L = _scriptHost.GetLuaState();

    lua_pushinteger(L, (ptrdiff_t)this);
    lua_setglobal(L, "dusk_current_ScriptComponent");

    _scriptHost.RunFile(_filename);
}

} // namespace dusk
