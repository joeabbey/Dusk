#ifndef DUSK_SCRIPT_HOST_HPP
#define DUSK_SCRIPT_HOST_HPP

#include <dusk/Config.hpp>

#include <unordered_map>

namespace dusk {

class ScriptHost
{
public:

    DISALLOW_COPY_AND_ASSIGN(ScriptHost);

    ScriptHost();
    virtual ~ScriptHost();

    bool RunFile(const std::string& filename);
    bool RunString(const std::string& code);

    lua_State * GetLuaState() const { return _luaState; }

    static bool AddFunction(const std::string& funName, lua_CFunction function);

private:

    static std::vector<ScriptHost *> _ScriptHosts;
    static std::unordered_map<std::string, lua_CFunction> _Functions;

    lua_State * _luaState;

}; // class ScriptHost

} // namespace dusk

#endif // DUSK_SCRIPT_HOST_HPP
