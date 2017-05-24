#include "dusk/Component.hpp"

#include <dusk/Log.hpp>
#include <dusk/App.hpp>

namespace dusk {

Component::Component()
    : _loaded(false)
{ }

Component::~Component()
{
}

MeshComponent::MeshComponent(const std::string& filename, Shader * shader)
    : Component()
    , _shaderData()
    , _shader(shader)
    , _filename(filename)
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

    // TODO: Move out of MeshComponent


    // TODO: Move out of Update
    _shaderData.model = glm::mat4(1);
    _shaderData.view = glm::lookAt(glm::vec3(5), glm::vec3(0), glm::vec3(0, 1, 0));
    _shaderData.proj = glm::perspective(45.0f, (float)App::Inst()->WindowWidth / (float)App::Inst()->WindowHeight, 0.1f, 100.0f);
    _shaderData.mvp = _shaderData.proj * _shaderData.view * _shaderData.model;

    _shader->SetData("TransformData", &_shaderData, sizeof(_shaderData));
}

void MeshComponent::Render()
{
    if (!_loaded) return;

    _shader->Bind();
    _mesh->Render();
}

ScriptComponent::ScriptComponent(const std::string& filename)
    : Component()
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
