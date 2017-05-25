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


} // namespace dusk
