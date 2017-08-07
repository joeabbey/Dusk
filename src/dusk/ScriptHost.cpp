#include "dusk/ScriptHost.hpp"

#include <dusk/App.hpp>
#include <dusk/Log.hpp>

namespace dusk {

ScriptHost::ScriptHost()
{
    _lua.open_libraries(sol::lib::base, sol::lib::math);
    _lua.new_usertype<ScriptHost>("ScriptHost",
        "EvtCleanup", &ScriptHost::EvtCleanup
    );

    _lua["this_script_host"] = this;

    App::LuaSetup(_lua);
}

ScriptHost::~ScriptHost()
{
    if (_lua["cleanup"].valid())
    {
        _lua["cleanup"]();
    }

    EvtCleanup.Call();
}

} // namespace dusk
