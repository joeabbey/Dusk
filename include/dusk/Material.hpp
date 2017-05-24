#ifndef DUSK_MATERIAL_HPP
#define DUSK_MATERIAL_HPP

#include <dusk/Config.hpp>

#include <dusk/Texture.hpp>

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

    bool Load();
    void Free();

    void Bind();

private:

    glm::vec4 _ambient;
    glm::vec4 _diffuse;
    glm::vec4 _specular;

    float _shininess;
    float _dissolve;

    Texture * _ambientMap;
    Texture * _diffuseMap;
    Texture * _specularMap;
    Texture * _bumpMap;

    MaterialData _shaderData;

};

} // namespace dusk

#endif // DUSK_MATERIAL_HPP
