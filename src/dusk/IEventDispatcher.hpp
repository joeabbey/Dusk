#ifndef DUSK_IEVENT_DISPATCHER_HPP
#define DUSK_IEVENT_DISPATCHER_HPP

#include <dusk/Config.hpp>

namespace dusk {

template <typename ReturnType, typename Param = void>
class ICallback
{
public:

    virtual ReturnType Invoke(Param) = 0;
    virtual bool IsMethodOf(void * object) = 0;

}; // class ICallback

template <typename ReturnType, typename Param = void>
class FunctionCallback : public ICallback<ReturnType, Param>
{
public:

    FunctionCallback(ReturnType (*function)(Param))
        : _function(function)
    { }

    inline virtual ReturnType Invoke(Param param) { return (*_function)(param); }
    inline virtual bool IsMethodOf(void * object) { return false; }

private:

    ReturnType (*_function)(Param);

}; // class FunctionCallback

template <typename ReturnType, typename Param = void,
          typename ObjectType = void, typename Method = void>
class MethodCallback : public ICallback<ReturnType, Param>
{
public:

    MethodCallback(void * object, Method method)
        : _object(object)
        , _method(method)
    { }

    inline virtual ReturnType Invoke(Param param)
        { return (static_cast<ObjectType *>(_object)->*_method)(param); }

    inline virtual bool IsMethodOf(void * object) { return object == _object; }

private:

    void * _object;
    Method _method;

}; // class MethodCallback

template <typename ReturnType, typename Param = void>
class LuaCallback : public ICallback<ReturnType, Param>
{
public:

    LuaCallback(ScriptHost * scriptHost, const std::string& funcName)
        : _scriptHost(scriptHost)
        , _funcName(funcName)
    { }

    // No General Invoke

    inline virtual void Invoke(const Event& event)
    {
        lua_State* L = _scriptHost->GetLuaState();

        lua_getglobal(L, _callback.c_str());

        int argCount = event.ToLua(L);

        lua_pcall(L, argCount, 0, 0);
    }

    inline virtual bool IsMethodOf(void * object) { return object == _object; }

private:

    ScriptHost * _scriptHost;
    std::string _funcName;

}; // class MethodCallback

template <typename ReturnType, typename Param = void>
class Delegate
{
public:

    DISALLOW_COPY_AND_ASSIGN(Delegate);

    Delegate(ReturnType(*function)(Param))
    {
        _callback = new FunctionCallback<ReturnType, Param>(function);
    }

    template <typename ObjectType, typename Method>
    Delegate(ObjectType * object, Method method)
    {
        _callback = new MethodCallback<ReturnType, Param, ObjectType, Method>(object, method);
    }

    Delegate(ScriptHost * scriptHost, const std::string& funcName)
    {
        _callback = new LuaCallback(scriptHost, funcName);
    }

    virtual inline ~Delegate() { delete _callback; }

    inline ReturnType Invoke(Param param) { return _callback->Invoke(param); }
    inline ReturnType operator()(Param param) { return Invoke(param); }

    inline bool IsMethodOf(void * object)
    {
        if (!_callback) return false;
        return _callback->IsMethodOf(object);
    }

private:

    ICallback<ReturnType, Param> * _callback;

}; // class Delegate

typedef Delegate<void, Event&> EventDelegate;

class IEventDispatcher
{
public:

    void AddEventListener(const EventID& eventId, const EventDelegate& delegate);

    void RemoveEventListener(const EventID& eventId, const EventDelegate& delegate);

    void AddEventListener(const EventID& eventId, void (*function)(const Event&))
    {
        AddEventListener(eventId, EventDelegate(function));
    }

    void RemoveEventListener(const EventID& eventId, void (*function)(const Event&))
    {
        RemoveEventListener(eventId, EventDelegate(function));
    }

    template <typename ObjectType, typename Method>
    void AddEventListener(const EventID& eventId, ObjectType * object, Method method)
    {
        AddEventListener(eventId, EventDelegate(object, method));
    }

    template <typename ObjectType, typename Method>
    void RemoveEventListener(const EventID& eventId, ObjectType * object, Method method)
    {
        RemoveEventListener(eventId, EventDelegate(object, method));
    }

    void DispatchEvent(const Event& event);

    void RemoveAllEventListeners();
    void RemoveAllEventListeners(const EventID& eventId);

private:

    std::unordered_map<EventID, std::vector<EventDelegate*>> _eventMap;

}; // class IEventDispatcher

} // namespace dusk

#endif // DUSK_IEVENT_DISPATCHER_HPP
