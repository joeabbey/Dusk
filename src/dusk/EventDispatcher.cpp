#include "dusk/EventDispatcher.hpp"

namespace dusk {

void IEventDispatcher::AddEventListener(const EventID& eventId, IEventCallback * callback)
{
    const auto& it = _eventListeners.find(eventId);
    if (it == _eventListeners.end())
    {
        _eventListeners.emplace(eventId, std::vector<IEventCallback *>());
    }

    _eventListeners[eventId].push_back(callback);
}

void IEventDispatcher::RemoveEventListener(const EventID& eventId, IEventCallback * callback)
{
    const auto& listIt = _eventListeners.find(eventId);
    if (listIt == _eventListeners.end()) return;

    for (auto it = listIt->second.begin(); it != listIt->second.end(); ++it)
    {
        IEventCallback * tmp = *it;
        if (*tmp == *callback)
        {
            delete tmp;
            std::swap(*it, listIt->second.back());
            listIt->second.pop_back();
            return;
        }
    }
}

void IEventDispatcher::DispatchEvent(const Event& event)
{
    const auto& listIt = _eventListeners.find(event.GetID());
    if (listIt == _eventListeners.end()) return;

    for (const auto& listener : listIt->second)
    {
        listener->Invoke(event);
    }
}

void IEventDispatcher::RemoveAllEventListeners()
{
    for (auto it : _eventListeners)
    {
        for (IEventCallback * callback : it.second)
        {
            delete callback;
        }
        it.second.clear();
    }
    _eventListeners.clear();
}

void IEventDispatcher::RemoveAllEventListeners(const EventID& eventId)
{
    const auto& listIt = _eventListeners.find(eventId);
    if (listIt == _eventListeners.end()) return;

    for (IEventCallback * callback : listIt->second)
    {
        delete callback;
    }
    listIt->second.clear();
}

void IEventDispatcher::InitScripting()
{
    ScriptHost::AddFunction("dusk_IEventDispatcher_AddEventListener",
                            &IEventDispatcher::Script_AddEventListener);
    ScriptHost::AddFunction("dusk_IEventDispatcher_RemoveEventListener",
                            &IEventDispatcher::Script_RemoveEventListener);
}

int IEventDispatcher::Script_AddEventListener(lua_State * L)
{
    IEventDispatcher * disp = (IEventDispatcher *)lua_tointeger(L, 1);

    disp->AddEventListener((EventID)lua_tointeger(L, 2),
                           new LuaEventCallback(L, std::string(lua_tostring(L, 3))));

    return 0;
}

int IEventDispatcher::Script_RemoveEventListener(lua_State * L)
{
    IEventDispatcher * disp = (IEventDispatcher *)lua_tointeger(L, 1);

    LuaEventCallback tmp(L, std::string(lua_tostring(L, 3)));
    disp->AddEventListener((EventID)lua_tointeger(L, 2), &tmp);

    return 0;
}


} // namespace dusk
