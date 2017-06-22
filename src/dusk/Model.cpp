#include "dusk/Model.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>
#include <dusk/App.hpp>
#include <dusk/Camera.hpp>

namespace dusk {

Model::Model(Shader * shader)
    : _shader(shader)
    , _baseTransform(1)
    , _transform(1)
    , _position(0)
    , _rotation(0)
    , _scale(1)
{
    memset(&_shaderData, 0, sizeof(_shaderData));
    Shader::AddData("DuskTransformData", &_shaderData, sizeof(_shaderData));
    _shader->BindData("DuskTransformData");
}

Model::~Model()
{

}

std::unique_ptr<Model> Model::Parse(nlohmann::json & data)
{
    App * app = App::GetInst();
    Shader * shader = app->GetShader(data["Shader"]);

    std::unique_ptr<Model> model(new Model(shader));

    for (auto& mesh : data["Meshes"])
    {
        model->AddMesh(Mesh::Parse(mesh));
    }

    return model;
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

void Model::Update()
{
    // TODO: Move camera somewhere else
    Camera * camera = App::GetInst()->GetScene()->GetCurrentCamera();

    _shaderData.model = GetTransform();
    _shaderData.view = camera->GetView();
    _shaderData.proj = camera->GetProjection();
    _shaderData.mvp = _shaderData.proj * _shaderData.view * _shaderData.model;
}

void Model::Render()
{
    _shader->Bind();

    Shader::UpdateData("DuskTransformData", &_shaderData, sizeof(_shaderData));

    for (auto& mesh : _meshes)
    {
        mesh->Render(_shader);
    }
}

} // namespace dusk
