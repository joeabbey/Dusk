#include "dusk/Material.hpp"

#include <dusk/Log.hpp>
#include <dusk/Shader.hpp>
#include <dusk/App.hpp>
#include <sstream>

namespace dusk {

std::shared_ptr<Material>
Material::Parse(nlohmann::json & data)
{
    glm::vec4 ambient  = glm::vec4(0, 0, 0, 1.0f);
    glm::vec4 diffuse  = glm::vec4(0, 0, 0, 1.0f);
    glm::vec4 specular = glm::vec4(0, 0, 0, 1.0f);
    float shininess = 0.0f;
    float dissolve = 0.0f;
    std::string ambientMap;
    std::string diffuseMap;
    std::string specularMap;
    std::string bumpMap;

    if (data.find("Ambient") != data.end())
    {
        ambient.x = data["Ambient"][0];
        ambient.y = data["Ambient"][1];
        ambient.z = data["Ambient"][2];
    }

    if (data.find("Diffuse") != data.end())
    {
        diffuse.x = data["Diffuse"][0];
        diffuse.y = data["Diffuse"][1];
        diffuse.z = data["Diffuse"][2];
    }

    if (data.find("Specular") != data.end())
    {
        specular.x = data["Specular"][0];
        specular.y = data["Specular"][1];
        specular.z = data["Specular"][2];
    }

    if (data.find("Shininess") != data.end())
    {
        shininess = data["Shininess"];
    }

    if (data.find("Dissolve") != data.end())
    {
        dissolve = data["Dissolve"];
    }

    if (data.find("AmbientMap") != data.end())
    {
        ambientMap = data["AmbientMap"];
    }

    if (data.find("DiffuseMap") != data.end())
    {
        diffuseMap = data["DiffuseMap"];
    }

    if (data.find("SpecularMap") != data.end())
    {
        specularMap = data["SpecularMap"];
    }

    if (data.find("BumpMap") != data.end())
    {
        bumpMap = data["BumpMap"];
    }

    return Material::Create(ambient, diffuse, specular,
                            shininess, dissolve,
                            ambientMap, diffuseMap, specularMap, bumpMap);
}

std::shared_ptr<Material>
Material::Create(glm::vec4 ambient,
                 glm::vec4 diffuse,
                 glm::vec4 specular,
                 float shininess,
                 float dissolve,
                 const std::string& ambientMap,
                 const std::string& diffuseMap,
                 const std::string& specularMap,
                 const std::string& bumpMap)
{
    std::stringstream ss;
    ss << "Material[" << ambient.r << "," << ambient.g << "," << ambient.b << "," << ambient.a << ","
                      << diffuse.r << "," << diffuse.g << "," << diffuse.b << "," << diffuse.a << ","
                      << specular.r << "," << specular.g << "," << specular.b << "," << specular.a << ","
                      << shininess << "," << dissolve << ","
                      << ambientMap << ","
                      << diffuseMap << ","
                      << specularMap << ","
                      << bumpMap << "]";

    App * app = App::GetInst();
    AssetId id = app->GetMaterialIndex()->GetId(ss.str());
    std::shared_ptr<Material> ptr = app->GetMaterialCache()->Get(id);
    if (!ptr)
    {
        ptr.reset(new Material(ambient, diffuse, specular,
                               shininess, dissolve,
                               ambientMap, diffuseMap, specularMap, bumpMap));
        app->GetMaterialCache()->Add(id, ptr);
    }
    return ptr;
}

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
        _ambientMap = Texture::Create(ambientMap);
    }
    if (!diffuseMap.empty())
    {
        _diffuseMap = Texture::Create(diffuseMap);
    }
    if (!specularMap.empty())
    {
        _specularMap = Texture::Create(specularMap);
    }
    if (!bumpMap.empty())
    {
        _bumpMap = Texture::Create(bumpMap);
    }

    _shaderData.Ambient  = _ambient;
    _shaderData.Diffuse  = _diffuse;
    _shaderData.Specular = _specular;

    _shaderData.Shininess = _shininess;
    _shaderData.Dissolve  = _dissolve;

    _shaderData.MapFlags = 0;
    _shaderData.MapFlags |= (ambientMap.empty()  ? 0 : AMBIENT_MAP_FLAG);
    _shaderData.MapFlags |= (diffuseMap.empty()  ? 0 : DIFFUSE_MAP_FLAG);
    _shaderData.MapFlags |= (specularMap.empty() ? 0 : SPECULAR_MAP_FLAG);
    _shaderData.MapFlags |= (bumpMap.empty()     ? 0 : BUMP_MAP_FLAG);
}

Material::~Material()
{
}

void Material::Bind(Shader * shader)
{
    Shader::UpdateData("DuskMaterialData", &_shaderData, sizeof(_shaderData));

    if (_ambientMap)
    {
        glUniform1i(shader->GetUniformLocation("_AmbientMap"), Material::TextureID::AMBIENT);
        glActiveTexture(GL_TEXTURE0 + Material::TextureID::AMBIENT);
        _ambientMap->Bind();
    }

    if (_diffuseMap)
    {
        glUniform1i(shader->GetUniformLocation("_DiffuseMap"), Material::TextureID::DIFFUSE);
        glActiveTexture(GL_TEXTURE0 + Material::TextureID::DIFFUSE);
        _diffuseMap->Bind();
    }

    if (_specularMap)
    {
        glUniform1i(shader->GetUniformLocation("_SpecularMap"), Material::TextureID::SPECULAR);
        glActiveTexture(GL_TEXTURE0 + Material::TextureID::SPECULAR);
        _specularMap->Bind();
    }

    if (_bumpMap)
    {
        glUniform1i(shader->GetUniformLocation("_BumpMap"), Material::TextureID::BUMP);
        glActiveTexture(GL_TEXTURE0 + Material::TextureID::BUMP);
        _bumpMap->Bind();
    }

    glActiveTexture(0);
}

} // namespace dusk
