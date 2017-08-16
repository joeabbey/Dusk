#ifndef DUSK_MODEL_HPP
#define DUSK_MODEL_HPP

#include <dusk/Config.hpp>

#include <dusk/Mesh.hpp>
#include <memory>

namespace dusk
{

struct TransformData
{
    alignas(64) glm::mat4 Model = glm::mat4(1);
    alignas(64) glm::mat4 View  = glm::mat4(1);
    alignas(64) glm::mat4 Proj  = glm::mat4(1);
    alignas(64) glm::mat4 MVP   = glm::mat4(1);
};

class Model
{
public:

    DISALLOW_COPY_AND_ASSIGN(Model);

    static void LuaSetup(sol::state& lua);

    Model();
    virtual ~Model() = default;

    void AddMesh(std::shared_ptr<Mesh> mesh);

    void SetBaseTransform(const glm::mat4& baseTransform);

    void SetPosition(const glm::vec3& pos);
    glm::vec3 GetPosition() const { return _position; }

    void SetRotation(const glm::vec3& rot);
    glm::vec3 GetRotation() const { return _rotation; }

    void SetScale(const glm::vec3& scale);
    glm::vec3 GetScale() const { return _scale; }

    glm::mat4 GetTransform();

    Box GetBounds() const { return _bounds; }

    virtual void Update(const UpdateContext& ctx);
    virtual void Render(RenderContext& ctx);

private:

    TransformData _shaderData;

    glm::mat4 _baseTransform;
    glm::mat4 _transform;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    Box _bounds;

    std::vector<std::shared_ptr<Mesh>> _meshes;

}; // class Model

} // namespace dusk

#endif // DUSK_MODEL_HPP
