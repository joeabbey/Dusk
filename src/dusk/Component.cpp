#include "dusk/Component.hpp"

#include <dusk/Log.hpp>
#include <dusk/App.hpp>
#include <dusk/Actor.hpp>

namespace dusk {

Component::Component(Actor * parent)
    : _loaded(false)
    , _parent(parent)
{
}

Component::~Component()
{
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

MeshComponent::MeshComponent(Actor * parent, Mesh * mesh)
    : Component(parent)
    , _mesh(mesh)
{
    GetActor()->AddComponentType<MeshComponent>(this);
    GetActor()->AddEventListener((EventID)Actor::Events::UPDATE, this, &MeshComponent::Update);
    GetActor()->AddEventListener((EventID)Actor::Events::RENDER, this, &MeshComponent::Render);
}

MeshComponent::~MeshComponent()
{
    GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &MeshComponent::Update);
    GetActor()->RemoveEventListener((EventID)Actor::Events::RENDER, this, &MeshComponent::Render);

    if (IsLoaded())
    {
        Free();
    }
}

bool MeshComponent::Load()
{
    if (!_mesh->Load())
    {
        DuskLogError("Failed to load mesh");
        return false;
    }

    _loaded = true;

    return true;
}

void MeshComponent::Free()
{
    _mesh->Free();
    _loaded = false;
}

void MeshComponent::Update(const Event& event)
{
    if (!_loaded) return;

    _mesh->SetBaseTransform(GetActor()->GetTransform());
    _mesh->Update();
}

void MeshComponent::Render(const Event& event)
{
    if (!_loaded) return;

    _mesh->Render();
}

CameraComponent::CameraComponent(Actor * parent, Camera * camera)
    : Component(parent)
    , _camera(camera)
{
    GetActor()->AddComponentType<CameraComponent>(this);
    GetActor()->AddEventListener((EventID)Actor::Events::UPDATE, this, &CameraComponent::Update);
}

CameraComponent::~CameraComponent()
{
    GetActor()->RemoveEventListener((EventID)Actor::Events::UPDATE, this, &CameraComponent::Update);

    delete _camera;

    if (IsLoaded())
    {
        Free();
    }
}

bool CameraComponent::Load()
{
    _loaded = true;

    return true;
}

void CameraComponent::Free()
{
}

void CameraComponent::Update(const Event& event)
{
    if (!_loaded) return;

    _camera->SetBaseTransform(glm::inverse(GetActor()->GetTransform()));
}

ScriptComponent::ScriptComponent(Actor * parent, const std::string& filename)
    : Component(parent)
    , _scriptHost()
    , _filename(filename)
{
    GetActor()->AddComponentType<ScriptComponent>(this);
}

ScriptComponent::~ScriptComponent()
{
    if (IsLoaded())
    {
        Free();
    }
}

bool ScriptComponent::Load()
{
    if (!_scriptHost.Load())
    {
        return false;
    }

    lua_State * L = _scriptHost.GetLuaState();

    lua_pushinteger(L, (ptrdiff_t)this);
    lua_setglobal(L, "dusk_current_ScriptComponent");

    _scriptHost.RunFile(_filename);

    _loaded = true;

    return true;
}

void ScriptComponent::Free()
{
    _scriptHost.Free();
}

} // namespace dusk
