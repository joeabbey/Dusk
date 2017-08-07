#ifndef DUSK_SCRIPT_HOST_HPP
#define DUSK_SCRIPT_HOST_HPP

#include <dusk/Config.hpp>

#include <dusk/Event.hpp>
#include <unordered_map>
#include <memory>

namespace dusk {

class ScriptHost
{
public:

    DISALLOW_COPY_AND_ASSIGN(ScriptHost);

    ScriptHost();
    virtual ~ScriptHost();

    void RunFile(const std::string& filename)
    {
        _lua.script_file(filename);
    }

    void RunCode(const std::string& code)
    {
        _lua.script(code);
    }

    lua_State * GetLuaState()
    {
        return _lua.lua_state();
    }

    Event<> EvtCleanup;

private:

    sol::state _lua;

}; // class ScriptHost

} // namespace dusk

#endif // DUSK_SCRIPT_HOST_HPP
