#include "dusk/Material.hpp"

#include <dusk/Log.hpp>
#include <dusk/Shader.hpp>
#include <dusk/App.hpp>
#include <sstream>

namespace dusk {

Material::Material(const Data& data)
    : _ambient(data.Ambient)
    , _diffuse(data.Diffuse)
    , _specular(data.Specular)
    , _shininess(data.Shininess)
    , _dissolve(data.Dissolve)
    , _ambientMap(nullptr)
    , _diffuseMap(nullptr)
    , _specularMap(nullptr)
    , _bumpMap(nullptr)
{
    if (!data.AmbientMap.empty())
    {
        _ambientMap.reset(new Texture(data.AmbientMap));
    }
    if (!data.DiffuseMap.empty())
    {
        _diffuseMap.reset(new Texture(data.DiffuseMap));
    }
    if (!data.SpecularMap.empty())
    {
        _specularMap.reset(new Texture(data.SpecularMap));
    }
    if (!data.BumpMap.empty())
    {
        _bumpMap.reset(new Texture(data.BumpMap));
    }

    _shaderData.Ambient  = _ambient;
    _shaderData.Diffuse  = _diffuse;
    _shaderData.Specular = _specular;

    _shaderData.Shininess = _shininess;
    _shaderData.Dissolve  = _dissolve;

    _shaderData.MapFlags = 0;
    _shaderData.MapFlags |= (_ambientMap && _ambientMap->IsLoaded()   ? AMBIENT_MAP_FLAG : 0);
    _shaderData.MapFlags |= (_diffuseMap && _diffuseMap->IsLoaded()   ? DIFFUSE_MAP_FLAG : 0);
    _shaderData.MapFlags |= (_specularMap && _specularMap->IsLoaded() ? SPECULAR_MAP_FLAG : 0);
    _shaderData.MapFlags |= (_bumpMap && _bumpMap->IsLoaded()         ? BUMP_MAP_FLAG : 0);
}

void Material::Bind(ShaderProgram * sp)
{
    ShaderProgram::SetUniformBufferData("DuskMaterialData", &_shaderData);

    if (_ambientMap && _ambientMap->IsLoaded())
    {
        sp->SetUniform("_AmbientMap", TextureID::AMBIENT);
        glActiveTexture(GL_TEXTURE0 + TextureID::AMBIENT);
        _ambientMap->Bind();
    }

    if (_diffuseMap && _diffuseMap->IsLoaded())
    {
        sp->SetUniform("_DiffuseMap", TextureID::DIFFUSE);
        glActiveTexture(GL_TEXTURE0 + TextureID::DIFFUSE);
        _diffuseMap->Bind();
    }

    if (_specularMap && _specularMap->IsLoaded())
    {
        sp->SetUniform("_SpecularMap", TextureID::SPECULAR);
        glActiveTexture(GL_TEXTURE0 + TextureID::SPECULAR);
        _specularMap->Bind();
    }

    if (_bumpMap && _bumpMap->IsLoaded())
    {
        sp->SetUniform("_BumpMap", TextureID::BUMP);
        glActiveTexture(GL_TEXTURE0 + TextureID::BUMP);
        _bumpMap->Bind();
    }

    glActiveTexture(0);
}

} // namespace dusk
