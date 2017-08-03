#ifndef DUSK_EVENT_HPP
#define DUSK_EVENT_HPP

#include <dusk/Config.hpp>

#include <memory>
#include <vector>
#include <functional>

namespace dusk {

template <class ... Params>
class Event
{
public:

    DISALLOW_COPY_AND_ASSIGN(Event);

    Event() = default;
    virtual ~Event() = default;

    unsigned int GetNextId()
    {
        return ++_nextId;
    }

    unsigned int AddStatic(std::function<void(Params ...)> func)
    {
        unsigned int id = GetNextId();
        _callbacks.push_back(std::make_unique<StaticCallback>(id, func));
        return id;
    }

    template <class Object>
    unsigned int AddMember(Object * object, std::function<void(Object *, Params ...)> func)
    {
        unsigned int id = GetNextId();
        _callbacks.push_back(std::make_unique<MemberCallback<Object>>(id, object, func));
        return id;
    }

    void Call(Params ... args)
    {
        for (auto& callback : _callbacks)
        {
            if (callback->alive)
                callback->Call(args...);
        }

        for (unsigned int i = 0; i < _callbacks.size(); ++i)
        {
            if (!_callbacks[i]->alive)
            {
                _callbacks.erase(_callbacks.begin() + i);
                --i;
            }
        }
    }

    void RemoveAllListeners()
    {
        for (auto& callback : _callbacks)
        {
            callback->alive = false;
        }
    }

    void RemoveListener(unsigned int id)
    {
        for (auto& callback : _callbacks)
        {
            if (callback->id == id)
            {
                callback->alive = false;
            }
        }
    }

private:

    struct Callback
    {
        bool alive;
        unsigned int id;

        Callback(unsigned int id)
            : alive(true)
            , id(id)
        { }

        virtual void Call(Params ... args) = 0;
    };

    struct StaticCallback : public Callback
    {
        std::function<void(Params ...)> callback;

        StaticCallback(unsigned int id, std::function<void(Params ...)> func)
            : Callback(id)
            , callback(func)
        { }

        void Call(Params ... args) override
        {
            callback(args...);
        }
    };

    template <class Object>
    struct MemberCallback : public Callback
    {
        Object * object;
        std::function<void(Object *, Params ...)> callback;

        MemberCallback(unsigned int id, Object * object, std::function<void(Object *, Params ...)> func)
            : object(object)
            , callback(func)
        { }

        void Call(Params ... args) override
        {
            callback(object, args...);
        }
    };

    unsigned int _nextId = 0;

    std::vector<std::unique_ptr<Callback>> _callbacks;

}; // class Event

} // namespace dusk

#endif // DUSK_EVENT_HPP
