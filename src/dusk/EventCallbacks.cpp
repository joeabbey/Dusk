#include "dusk/EventCallbacks.hpp"

namespace dusk {

void LuaEventCallback::Invoke(const Event& event)
{
    lua_getglobal(_luaState, _funcName.c_str());

    int argCount = event.PushDataToLua(_luaState);

    lua_pcall(_luaState, argCount, 0, 0);
}

} // namespace dusk
