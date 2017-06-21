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
	if ("Mesh" == type)
	{
		component.reset(new MeshComponent(actor, Mesh::Parse(data)));
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

    //MeshComponent::InitScripting();
    //CameraComponent::InitScripting();
    //ScriptComponent::InitScripting();
}

int Component::Script_GetActor(lua_State * L)
{
    Component * componenet = (Component *)lua_tointeger(L, 1);

    lua_pushinteger(L, (ptrdiff_t)componenet->GetActor());

    return 1;
}

MeshComponent::MeshComponent(Actor * parent, std::shared_ptr<Mesh> mesh)
    : Component(parent)
    , _mesh(mesh)
{
    GetActor()->AddEventListener((EventID)Actor::Events::UPDATE, this, &MeshComponent::Update);
    GetActor()->AddEventListener((EventID)Actor::Events::RENDER, this, &MeshComponent::Render);
}

MeshComponent::~MeshComponent()
{
    GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &MeshComponent::Update);
    GetActor()->RemoveEventListener((EventID)Actor::Events::RENDER, this, &MeshComponent::Render);
}

void MeshComponent::Update(const Event& event)
{
    _mesh->SetBaseTransform(GetActor()->GetTransform());
    _mesh->Update();
}

void MeshComponent::Render(const Event& event)
{
    _mesh->Render();
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
