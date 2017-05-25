#include "dusk/ScriptHost.hpp"

namespace dusk {

std::vector<ScriptHost *> ScriptHost::_ScriptHosts;
std::unordered_map<std::string, lua_CFunction> ScriptHost::_Functions;

ScriptHost::ScriptHost()
{
    _ScriptHosts.push_back(this);

    _luaState = luaL_newstate();
    if (!_luaState)
    {
        DuskLogError("Failed to create Lua state");
        return;
    }

    luaL_openlibs(_luaState);

    for (const auto& it : _Functions)
    {
        lua_register(_luaState, it.first.c_str(), it.second);
    }
}

ScriptHost::~ScriptHost()
{
    auto it = std::find(_ScriptHosts.begin(), _ScriptHosts.end(), this);
    if (it != _ScriptHosts.end())
    {
        std::swap(*it, _ScriptHosts.back());
        _ScriptHosts.pop_back();
    }

    lua_close(_luaState);
}

bool ScriptHost::RunFile(const std::string& filename)
{
    int status = luaL_loadfile(_luaState, filename.c_str());
    if (status)
        goto error;

    // sets the error callback to 0, this means errors will be pushed onto the stack
    status = lua_pcall(_luaState, 0, LUA_MULTRET, 0);

    if (status)
        goto error;

    return true;

error:

    // get error message from stack
    DuskLogError("Failed to run script '%s', %s", filename.c_str(), lua_tostring(_luaState, -1));
    // remove error message
    lua_pop(_luaState, 1);

    return false;
}

bool ScriptHost::RunString(const std::string& code)
{
    int status = luaL_loadbuffer(_luaState, code.c_str(), code.size(), NULL);
    if (status)
        goto error;

    // sets the error callback to 0, this means errors will be pushed onto the stack
    status = lua_pcall(_luaState, 0, LUA_MULTRET, 0);

    if (status)
        goto error;

    return true;

error:

    // get error message from stack
    DuskLogError("Failed to run script '%s', %s", filename.c_str(), lua_tostring(_luaState, -1));
    // remove error message
    lua_pop(_luaState, 1);

    return false;
}

bool ScriptHost::AddFunction(const std::string& funName, lua_CFunction function)
{
    if (funcName.empty())
    {
        DuskLogError("Cannot register a function with no name");
        return false;
    }

    if (!callback)
    {
        DuskLogError("Cannot register a null function");
        return false;
    }

    auto it = _Functions.find(funcName);
    if (it != _Functions.end())
    {
        DuskLogWarn("Cannot register a function twice");
        return false;
    }

    _Functions.emplace(funcName, function);
    for (const auto& it : _Functions)
    {
        lua_register(_luaState, funcName.c_str(), function);
    }
}

} // namespace dusk
