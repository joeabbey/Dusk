#include "dusk/Material.hpp"

#include <dusk/Log.hpp>
#include <dusk/Shader.hpp>

namespace dusk {

Material::Material(glm::vec4 ambient,
                   glm::vec4 diffuse,
                   glm::vec4 specular,
                   float shininess,
                   float dissolve,
                   const std::string& ambientMap,
                   const std::string& diffuseMap,
                   const std::string& specularMap,
                   const std::string& bumpMap)
    : _ambient(ambient)
    , _diffuse(diffuse)
    , _specular(specular)
    , _shininess(shininess)
    , _dissolve(dissolve)
    , _ambientMap(nullptr)
    , _diffuseMap(nullptr)
    , _specularMap(nullptr)
    , _bumpMap(nullptr)
{
    if (!ambientMap.empty())
    {
        _ambientMap = std::make_shared<Texture>(ambientMap);
    }
    if (!diffuseMap.empty())
    {
        _diffuseMap = std::make_shared<Texture>(diffuseMap);
    }
    if (!specularMap.empty())
    {
        _specularMap = std::make_shared<Texture>(specularMap);
    }
    if (!bumpMap.empty())
    {
        _bumpMap = std::make_shared<Texture>(bumpMap);
    }

    _shaderData.ambient  = _ambient;
    _shaderData.diffuse  = _diffuse;
    _shaderData.specular = _specular;

    _shaderData.shininess = _shininess;
    _shaderData.dissolve  = _dissolve;

    _shaderData.hasAmbientMap  = !ambientMap.empty();
    _shaderData.hasDiffuseMap  = !diffuseMap.empty();
    _shaderData.hasSpecularMap = !specularMap.empty();
    _shaderData.hasBumpMap     = !bumpMap.empty();
}

Material::~Material()
{
}

void Material::Bind(Shader * shader)
{
    // TODO: Move
    shader->Bind();

    Shader::UpdateData("DuskMaterialData", &_shaderData, sizeof(_shaderData));
    shader->BindData("DuskMaterialData");

    glUniform1i(shader->GetUniformLocation("_AmbientMap"), Material::TextureID::AMBIENT);
    glUniform1i(shader->GetUniformLocation("_DiffuseMap"), Material::TextureID::DIFFUSE);
    glUniform1i(shader->GetUniformLocation("_SpecularMap"), Material::TextureID::SPECULAR);
    glUniform1i(shader->GetUniformLocation("_BumpMap"), Material::TextureID::BUMP);

    if (_ambientMap)
    {
        glActiveTexture(GL_TEXTURE0 + Material::TextureID::AMBIENT);
        _ambientMap->Bind();
    }

    if (_diffuseMap)
    {
        glActiveTexture(GL_TEXTURE0 + Material::TextureID::DIFFUSE);
        _diffuseMap->Bind();
    }

    if (_specularMap)
    {
        glActiveTexture(GL_TEXTURE0 + Material::TextureID::SPECULAR);
        _specularMap->Bind();
    }

    if (_bumpMap)
    {
        glActiveTexture(GL_TEXTURE0 + Material::TextureID::BUMP);
        _bumpMap->Bind();
    }

    glActiveTexture(0);
}

} // namespace dusk
