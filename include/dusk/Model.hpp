#ifndef DUSK_MODEL_HPP
#define DUSK_MODEL_HPP

#include <dusk/Config.hpp>

#include <dusk/Mesh.hpp>
#include <memory>

namespace dusk
{

struct TransformData
{
    alignas(64) glm::mat4 model = glm::mat4(1);
    alignas(64) glm::mat4 view  = glm::mat4(1);
    alignas(64) glm::mat4 proj  = glm::mat4(1);
    alignas(64) glm::mat4 mvp   = glm::mat4(1);
};

class Model
{
public:

    DISALLOW_COPY_AND_ASSIGN(Model);

    Model(Shader * shader);
    virtual ~Model();

    static std::unique_ptr<Model> Parse(nlohmann::json & data);
    std::unique_ptr<Model> Clone();

    void AddMesh(std::shared_ptr<Mesh> mesh);

    void SetBaseTransform(const glm::mat4& baseTransform);

    void SetPosition(const glm::vec3& pos);
    glm::vec3 GetPosition() const { return _position; }

    void SetRotation(const glm::vec3& rot);
    glm::vec3 GetRotation() const { return _rotation; }

    void SetScale(const glm::vec3& scale);
    glm::vec3 GetScale() const { return _scale; }

    glm::mat4 GetTransform();

    virtual void Update();
    virtual void Render();

    /*
    static void InitScripting();

    static int Script_GetPosition(lua_State * L);
    static int Script_SetPosition(lua_State * L);
    static int Script_GetRotation(lua_State * L);
    static int Script_SetRotation(lua_State * L);
    static int Script_GetScale(lua_State * L);
    static int Script_SetScale(lua_State * L);
    */

private:

    Shader * _shader;

    TransformData _shaderData;

    glm::mat4 _baseTransform;
    glm::mat4 _transform;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    std::vector<std::shared_ptr<Mesh>> _meshes;

}; // class Model

} // namespace dusk

#endif // DUSK_MODEL_HPP
