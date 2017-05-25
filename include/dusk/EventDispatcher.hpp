#ifndef DUSK_IEVENT_DISPATCHER_HPP
#define DUSK_IEVENT_DISPATCHER_HPP

#include <dusk/Config.hpp>
#include <dusk/Event.hpp>
#include <dusk/EventCallbacks.hpp>

#include <unordered_map>

namespace dusk {

class IEventDispatcher
{
public:

    IEventDispatcher() = default;
    virtual ~IEventDispatcher()
    {
        RemoveAllEventListeners();
    }

    void AddEventListener(const EventID& eventId, IEventCallback * callback);

    void RemoveEventListener(const EventID& eventId, IEventCallback * callback);

    void AddEventListener(const EventID& eventId, void (*function)(const Event&))
    {
        AddEventListener(eventId, new FunctionEventCallback(function));
    }

    void RemoveEventListener(const EventID& eventId, void (*function)(const Event&))
    {
        FunctionEventCallback tmp(function);
        RemoveEventListener(eventId, &tmp);
    }

    template <typename ObjectType, typename MethodType>
    void AddEventListener(const EventID& eventId, ObjectType * object, MethodType method)
    {
        AddEventListener(eventId, new MethodEventCallback<ObjectType, MethodType>(object, method));
    }

    template <typename ObjectType, typename MethodType>
    void RemoveEventListener(const EventID& eventId, ObjectType * object, MethodType method)
    {
        MethodEventCallback<ObjectType, MethodType> tmp(object, method);
        RemoveEventListener(eventId, &tmp);
    }

    void DispatchEvent(const Event& event);

    void RemoveAllEventListeners();
    void RemoveAllEventListeners(const EventID& eventId);

private:

    std::unordered_map<EventID, std::vector<IEventCallback *>> _eventListeners;

}; // class IEventDispatcher

} // namespace dusk

#endif // DUSK_IEVENT_DISPATCHER_HPP
