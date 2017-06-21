#ifndef DUSK_MATERIAL_HPP
#define DUSK_MATERIAL_HPP

#include <dusk/Config.hpp>

#include <dusk/Texture.hpp>
#include <dusk/Shader.hpp>
#include <memory>

namespace dusk {

struct MaterialData
{
    alignas(16)  glm::vec4 ambient  = glm::vec4(0, 0, 0, 1);
    alignas(16)  glm::vec4 diffuse  = glm::vec4(0, 0, 0, 1);
    alignas(16)  glm::vec4 specular = glm::vec4(0, 0, 0, 1);

    alignas(4)   float shininess = 0.0f;
    alignas(4)   float dissolve  = 0.0f;

    alignas(4)   bool hasAmbientMap  = false;
    alignas(4)   bool hasDiffuseMap  = false;
    alignas(4)   bool hasSpecularMap = false;
    alignas(4)   bool hasBumpMap     = false;
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

    Material(glm::vec4 ambient,
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

private:

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
