#include "dusk/EventDispatcher.hpp"

#include <dusk/Log.hpp>

unsigned int balance = 0;

namespace dusk {

void IEventDispatcher::AddEventListener(const EventID& eventId, IEventCallback * callback)
{
    if (_eventListeners.find(eventId) == _eventListeners.end())
    {
        _eventListeners.emplace(eventId, std::vector<IEventCallback *>());
    }

    _eventListeners[eventId].push_back(callback);
}

void IEventDispatcher::RemoveEventListener(const EventID& eventId, const IEventCallback * callback)
{
    const auto& listIt = _eventListeners.find(eventId);
    if (listIt == _eventListeners.end()) return;

    auto& list = listIt->second;

    if (_deadEventListeners.find(eventId) == _deadEventListeners.end())
    {
        _deadEventListeners.emplace(eventId, std::vector<IEventCallback *>());
    }

    for (auto it = list.begin(); it != list.end(); ++it)
    {
        IEventCallback * tmp = *it;
        if (*tmp == *callback)
        {
            _deadEventListeners[eventId].push_back(tmp);
            return;
        }
    }
}

void IEventDispatcher::DispatchEvent(const Event& event)
{
    const auto& listIt = _eventListeners.find(event.GetID());
    if (listIt == _eventListeners.end()) return;

    auto& list = listIt->second;

    for (const auto& listener : list)
    {
        if (_deadEventListeners.find(event.GetID()) != _deadEventListeners.end())
        {
            DuskLogWarn("Has dead events");
            bool dead = false;
            for (IEventCallback * callback : _deadEventListeners[event.GetID()])
            {
                if (listener == callback)
                {
                    DuskLogWarn("Is dead");
                    dead = true;
                    continue;
                }
            }
            if (dead)
                continue;
        }
        listener->Invoke(event);
    }

    for (auto deadEventList : _deadEventListeners)
    {
        const EventID& eventId = deadEventList.first;
        auto& deadList = deadEventList.second;

        for (IEventCallback * deadCallback : deadList)
        {
            auto& eventList = _eventListeners[eventId];
            for (auto it = eventList.begin(); it != eventList.end(); ++it)
            {
                if (*it == deadCallback)
                {
                    DuskLogWarn("Murder");
                    delete deadCallback;
                    std::swap(*it, eventList.back());
                    eventList.pop_back();
                    break;
                }
            }
        }
    }
    _deadEventListeners.clear();
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
    _deadEventListeners.clear();
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
    if (_deadEventListeners.find(eventId) != _deadEventListeners.end())
    {
        _deadEventListeners[eventId].clear();
    }
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

    std::string funcName = lua_tostring(L, 3);
    disp->AddEventListener((EventID)lua_tointeger(L, 2),
                           new LuaEventCallback(L, funcName));

    return 0;
}

int IEventDispatcher::Script_RemoveEventListener(lua_State * L)
{
    IEventDispatcher * disp = (IEventDispatcher *)lua_tointeger(L, 1);

    DuskLogWarn("Removing Lua Event Listener");

    std::string funcName = lua_tostring(L, 3);
    LuaEventCallback tmp(L, funcName);
    disp->RemoveEventListener((EventID)lua_tointeger(L, 2), &tmp);

    return 0;
}


} // namespace dusk
