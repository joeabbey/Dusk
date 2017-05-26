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

MeshComponent::MeshComponent(Actor * parent, const std::string& filename, Shader * shader)
    : Component(parent)
    , _shaderData()
    , _shader(shader)
    , _filename(filename)
    , _position(0)
    , _rotation(0)
    , _scale(1)
    , _transform(1)
{
    _mesh = new Mesh(_filename);

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
    Shader::SetData("TransformData", &_shaderData, sizeof(_shaderData));
    _shader->BindData("TransformData");

    _shader->Bind();
    // TODO: Move out of MeshComponent
    glUniform1i(_shader->GetUniformLocation("ambient_map"), Material::TextureID::AMBIENT);
    glUniform1i(_shader->GetUniformLocation("diffuse_map"), Material::TextureID::DIFFUSE);
    glUniform1i(_shader->GetUniformLocation("specular_map"), Material::TextureID::SPECULAR);
    glUniform1i(_shader->GetUniformLocation("bump_map"), Material::TextureID::BUMP);

    if (!_mesh->Load())
    {
        DuskLogError("Failed to load mesh component");
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

    _transform = glm::mat4(1);
    _transform = glm::translate(_transform, _position);
    _transform = glm::rotate(_transform, _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    _transform = glm::scale(_transform, _scale);

    _transform = _parent->GetTransform() * _transform;
}

void MeshComponent::Render(const Event& event)
{
    if (!_loaded) return;

    Camera * camera = GetActor()->GetScene()->GetCamera();

    _shaderData.model = _transform;
    _shaderData.view = camera->GetView();
    _shaderData.proj = camera->GetProjection();
    _shaderData.mvp = _shaderData.proj * _shaderData.view * _shaderData.model;

    _shader->SetData("TransformData", &_shaderData, sizeof(_shaderData));

    _shader->Bind();
    _mesh->Render();
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
