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
    : _loaded(false)
    , _ambient(ambient)
    , _diffuse(diffuse)
    , _specular(specular)
    , _shininess(shininess)
    , _dissolve(dissolve)
    , _ambientMap(nullptr)
    , _diffuseMap(nullptr)
    , _specularMap(nullptr)
    , _bumpMap(nullptr)
    , _shaderData()
{
    if (!ambientMap.empty())
    {
        _ambientMap = new Texture(ambientMap);
    }
    if (!diffuseMap.empty())
    {
        _diffuseMap = new Texture(diffuseMap);
    }
    if (!specularMap.empty())
    {
        _specularMap = new Texture(specularMap);
    }
    if (!bumpMap.empty())
    {
        _bumpMap = new Texture(bumpMap);
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

    Shader::DefineData("MaterialData", sizeof(MaterialData));
}

Material::~Material()
{
    delete _ambientMap;
    _ambientMap = nullptr;

    delete _diffuseMap;
    _diffuseMap = nullptr;

    delete _specularMap;
    _specularMap = nullptr;

    delete _bumpMap;
    _bumpMap = nullptr;
}

bool Material::Load()
{
    bool retval = true;

    if (_ambientMap)
    {
        DuskLogInfo("Loading ambient map");
        retval &= _ambientMap->Load();
    }
    if (_diffuseMap)
    {
        DuskLogInfo("Loading diffuse map");
        retval &= _diffuseMap->Load();
    }
    if (_specularMap)
    {
        DuskLogInfo("Loading specular map");
        retval &= _specularMap->Load();
    }
    if (_bumpMap)
    {
        DuskLogInfo("Loading bump map");
        retval &= _bumpMap->Load();
    }

    _loaded = true;

    return retval;
}

void Material::Free()
{
    if (_ambientMap)
    {
        _ambientMap->Free();
    }
    if (_diffuseMap)
    {
        _diffuseMap->Free();
    }
    if (_specularMap)
    {
        _specularMap->Free();
    }
    if (_bumpMap)
    {
        _bumpMap->Free();
    }

    _loaded = false;
}

void Material::Bind()
{
    Shader::UpdateData("MaterialData", &_shaderData, sizeof(_shaderData));

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
