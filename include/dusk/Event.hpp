#ifndef DUSK_EVENT_HPP
#define DUSK_EVENT_HPP

#include <dusk/Config.hpp>

#include <dusk/Log.hpp>
#include <memory>
#include <vector>
#include <functional>

namespace dusk {

template <class ... Params>
class Event
{
public:

    DISALLOW_COPY_AND_ASSIGN(Event);

    static void LuaSetup(sol::state& lua, const std::string& name)
    {
        lua.new_usertype<Event<Params ...>>(name,
            "new", sol::no_constructor,
            "AddListener", [](Event<Params ...> * event,
                              std::function<void(Params ...)> func,
                              sol::this_state state) -> unsigned int {
                return event->AddLua(state, func);
            },
            "RemoveListener", &Event<Params ...>::RemoveListener,
            "RemoveAllListeners", &Event<Params ...>::RemoveAllListeners
        );
    }

    Event() = default;
    virtual ~Event()
    {
        for (auto& func : _cleanupFuncs)
        {
            func();
        }
    };

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

    unsigned int AddLua(lua_State * state, std::function<void(Params ...)> func)
    {
        sol::state_view lua(state);

        unsigned int id = GetNextId();
        _callbacks.push_back(std::make_unique<LuaCallback>(id, state, func));

        // Use Lua to avoid cross-include issue
        // Gross
        Event<>* evtCleanup = lua["this_script_host"]["EvtCleanup"];
        evtCleanup->AddStatic([=]() {
            RemoveListener(id);
        });

        return id;
    }

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

    struct LuaCallback : public Callback
    {
        lua_State * state;
        std::function<void(Params ...)> callback;

        LuaCallback(unsigned int id, lua_State * state, std::function<void(Params ...)> func)
            : Callback(id)
            , state(state)
            , callback(func)
        { }

        void Call(Params ... args) override
        {
            callback(args...);
        }
    };

    // Reserve 0 as invalid id
    unsigned int _nextId = 0;

    std::vector<std::unique_ptr<Callback>> _callbacks;
    std::vector<std::function<void()>> _cleanupFuncs;

}; // class Event

} // namespace dusk

#endif // DUSK_EVENT_HPP
