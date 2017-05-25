#ifndef DUSK_EVENT_HPP
#define DUSK_EVENT_HPP

#include <dusk/Config.hpp>

namespace dusk {

class IEventDispatcher;

typedef unsigned int EventID;

class EventData
{
public:

    static const EventData Empty;

    EventData() = default;
    EventData(const EventData&) = default;
    virtual ~EventData() = default;
    EventData& operator=(const EventData&) = default;

    virtual int PushToLua(lua_State * L) const;

}; // class EventData

class Event
{
public:

    DISALLOW_COPY_AND_ASSIGN(Event);

    Event(const EventID& eventId, const EventData& data = EventData::Empty)
        : _id(eventId)
        , _data(data)
        , _target(nullptr)
    { }

    int PushDataToLua(lua_State * L) const { return _data.PushToLua(L); }

    inline EventID GetID() const { return _id; }

    inline void SetTarget(IEventDispatcher * target) { _target = target; }
    inline IEventDispatcher * GetTarget() const { return _target; }

    inline const EventData * GetData() const { return &_data; }

    template <typename T>
    inline const T * GetDataAs() const { return dynamic_cast<const T *>(&_data); }

private:

    EventID _id;
    const EventData& _data;
    IEventDispatcher * _target;

}; // class Event

} // namespace dusk

#endif // DUSK_EVENT_HPP
