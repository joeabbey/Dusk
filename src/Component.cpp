#include "dusk/Component.hpp"

#include <dusk/Log.hpp>

namespace dusk {

Component::Component()
    : _loaded(false)
{ }

Component::~Component()
{
}

MeshComponent::MeshComponent(const std::string& file)
    : Component()
    , _file(file)
{ }

MeshComponent::~MeshComponent()
{
    if (IsLoaded())
    {
        Free();
    }
}

bool MeshComponent::Load()
{
    return true;
}

void MeshComponent::Free()
{

}

void MeshComponent::Update()
{

}

void MeshComponent::Render()
{

}

ScriptComponent::ScriptComponent(const std::string& file)
    : Component()
    , _file(file)
{ }

ScriptComponent::~ScriptComponent()
{
    if (IsLoaded())
    {
        Free();
    }
}

bool ScriptComponent::Load()
{
    return true;

    // TODO: Move
    lua_State * _luaState = NULL;

    int status = luaL_loadfile(_luaState, _file.c_str());

    if (status)
    {
        DuskLogError("Failed to load script '%s'", _file.c_str());
        return false;
    }

    DuskLogInfo("Executing script '%s'", _file.c_str());

    // Set the error callback to 0, this means errors will be pushed onto the stack
    status = lua_pcall(_luaState, 0, LUA_MULTRET, 0);

    if (status)
    {
        DuskLogError("Failed to execute script '%s'", _file.c_str());
        return false;
    }

    return true;
}

void ScriptComponent::Free()
{

}

void ScriptComponent::Update()
{

}

void ScriptComponent::Render()
{

}


} // namespace dusk
