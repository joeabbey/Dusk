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

MeshComponent::MeshComponent(Actor * parent, Mesh * mesh)
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

    _shader->Bind();

    Shader::UpdateData("TransformData", &_shaderData, sizeof(_shaderData));

    _mesh->Render();

    // TODO: Move
    _shader->BindData("MaterialData");
}

CameraComponent::CameraComponent(Actor * parent, Camera * camera)
    : Component(parent)
    , _camera(camera)
{
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
{ }

ScriptComponent::~ScriptComponent()
{
    if (IsLoaded())
    {
        Free();
    }
}

bool ScriptComponent::Load()
{
    _scriptHost.RunFile(_filename);

    _loaded = true;

    return true;
}

void ScriptComponent::Free()
{

}

} // namespace dusk
