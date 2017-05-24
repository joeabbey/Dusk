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
}

MeshComponent::~MeshComponent()
{
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

void MeshComponent::Update()
{
    if (!_loaded) return;

    _transform = glm::mat4(1);
    _transform = glm::translate(_transform, _position);
    _transform = glm::rotate(_transform, _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    _transform = glm::scale(_transform, _scale);

    _transform = _parent->GetTransform() * _transform;

    Camera * camera = GetActor()->GetScene()->GetCamera();

    _shaderData.model = _transform;
    _shaderData.view = camera->GetView();
    _shaderData.proj = camera->GetProjection();
    _shaderData.mvp = _shaderData.proj * _shaderData.view * _shaderData.model;

    _shader->SetData("TransformData", &_shaderData, sizeof(_shaderData));
}

void MeshComponent::Render()
{
    if (!_loaded) return;

    _shader->Bind();
    _mesh->Render();
}

ScriptComponent::ScriptComponent(Actor * parent, const std::string& filename)
    : Component(parent)
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
    return true;

    // TODO: Move
    lua_State * _luaState = NULL;

    int status = luaL_loadfile(_luaState, _filename.c_str());

    if (status)
    {
        DuskLogError("Failed to load script '%s'", _filename.c_str());
        return false;
    }

    DuskLogInfo("Executing script '%s'", _filename.c_str());

    // Set the error callback to 0, this means errors will be pushed onto the stack
    status = lua_pcall(_luaState, 0, LUA_MULTRET, 0);

    if (status)
    {
        DuskLogError("Failed to execute script '%s'", _filename.c_str());
        return false;
    }

    _loaded = true;

    return true;
}

void ScriptComponent::Free()
{

}

void ScriptComponent::Update()
{

}

void ScriptComponent::Render()
{

}


} // namespace dusk