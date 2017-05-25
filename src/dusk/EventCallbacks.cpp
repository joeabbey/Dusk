#include "dusk/EventCallbacks.hpp"

namespace dusk {

void LuaEventCallback::Invoke(const Event& event)
{
    lua_State* L = _scriptHost->GetLuaState();

    lua_getglobal(L, _funcName.c_str());

    int argCount = event.PushDataToLua(L);

    lua_pcall(L, argCount, 0, 0);
}

} // namespace dusk
