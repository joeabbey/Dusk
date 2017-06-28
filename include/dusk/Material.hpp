#ifndef DUSK_MATERIAL_HPP
#define DUSK_MATERIAL_HPP

#include <dusk/Config.hpp>

#include <dusk/Texture.hpp>
#include <dusk/Shader.hpp>
#include <memory>

namespace dusk {

struct MaterialData
{
    alignas(16)  glm::vec4 Ambient  = glm::vec4(0, 0, 0, 1);
    alignas(16)  glm::vec4 Diffuse  = glm::vec4(0, 0, 0, 1);
    alignas(16)  glm::vec4 Specular = glm::vec4(0, 0, 0, 1);

    alignas(4)   GLfloat Shininess = 0.0f;
    alignas(4)   GLfloat Dissolve  = 0.0f;

    alignas(4)   GLuint MapFlags = 0;
};

class Material
    : public std::enable_shared_from_this<Material>
{
public:

    DISALLOW_COPY_AND_ASSIGN(Material);

    enum TextureID : GLuint
    {
        AMBIENT  = 0,
        DIFFUSE  = 1,
        SPECULAR = 2,
        BUMP     = 3,
    };

    enum MapFlags : GLuint
    {
        AMBIENT_MAP_FLAG  = 1,
        DIFFUSE_MAP_FLAG  = 2,
        SPECULAR_MAP_FLAG = 4,
        BUMP_MAP_FLAG     = 8,
    };

    static std::shared_ptr<Material> Parse(nlohmann::json & data);

    static std::shared_ptr<Material>
    Create(glm::vec4 ambient,
           glm::vec4 diffuse,
           glm::vec4 specular,
           float shininess,
           float dissolve,
           const std::string& ambientMap,
           const std::string& diffuseMap,
           const std::string& specularMap,
           const std::string& bumpMap);

    ~Material();

    void Bind(Shader * shader);

    // TODO: Fix
    std::string GetId();

private:

    Material(glm::vec4 ambient,
             glm::vec4 diffuse,
             glm::vec4 specular,
             float shininess,
             float dissolve,
             const std::string& ambientMap,
             const std::string& diffuseMap,
             const std::string& specularMap,
             const std::string& bumpMap);

    MaterialData _shaderData;

    glm::vec4 _ambient;
    glm::vec4 _diffuse;
    glm::vec4 _specular;

    float _shininess;
    float _dissolve;

    std::shared_ptr<Texture> _ambientMap;
    std::shared_ptr<Texture> _diffuseMap;
    std::shared_ptr<Texture> _specularMap;
    std::shared_ptr<Texture> _bumpMap;

};

} // namespace dusk

#endif // DUSK_MATERIAL_HPP
