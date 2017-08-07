#include "dusk/Model.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>
#include <dusk/App.hpp>
#include <dusk/Camera.hpp>

namespace dusk {

void Model::LuaSetup(sol::state& lua)
{
    lua.new_usertype<Model>("Model",
        "AddMesh", &Model::AddMesh,
        "SetBaseTransform", &Model::SetBaseTransform,
        "GetPosition", &Model::GetPosition,
        "SetPosition", &Model::SetPosition,
        "GetRotation", &Model::GetRotation,
        "SetRotation", &Model::SetRotation,
        "GetScale", &Model::GetScale,
        "SetScale", &Model::SetScale,
        "GetTransform", &Model::GetTransform,
        "Update", &Model::Update,
        "Render", &Model::Render
    );
}

Model::Model()
    : _baseTransform(1)
    , _transform(1)
    , _position(0)
    , _rotation(0)
    , _scale(1)
{
}

void Model::AddMesh(std::shared_ptr<Mesh> mesh)
{
    _meshes.push_back(mesh);
}

glm::mat4 Model::GetTransform()
{
    _transform = _baseTransform;
    _transform = glm::scale(_transform, _scale);
    _transform = glm::rotate(_transform, _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    _transform = glm::translate(_transform, _position);

    return _transform;
}

void Model::SetBaseTransform(const glm::mat4& baseTransform)
{
    _baseTransform = baseTransform;
}

void Model::SetPosition(const glm::vec3& pos)
{
    _position = pos;
}

void Model::SetRotation(const glm::vec3& rot)
{
    _rotation = rot;
}

void Model::SetScale(const glm::vec3& scale)
{
    _scale = scale;
}

void Model::Update(const UpdateContext& ctx)
{
    // TODO: Move camera somewhere else
    //Camera * camera = App::GetInst()->GetScene()->GetCurrentCamera();

    _shaderData.Model = GetTransform();
}

void Model::Render(RenderContext& ctx)
{
    if (!ctx.CurrentShader || !ctx.CurrentCamera) return;

    _shaderData.View = ctx.CurrentCamera->GetView();
    _shaderData.Proj = ctx.CurrentCamera->GetProjection();
    _shaderData.MVP = _shaderData.Proj * _shaderData.View * _shaderData.Model;

    ShaderProgram::SetUniformBufferData("DuskTransformData", &_shaderData);

    for (auto& mesh : _meshes)
    {
        mesh->Render(ctx);
    }
}

} // namespace dusk
