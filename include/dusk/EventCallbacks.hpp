#ifndef DUSK_CALLBACKS_HPP
#define DUSK_CALLBACKS_HPP

#include <dusk/Config.hpp>
#include <dusk/ScriptHost.hpp>
#include <dusk/Event.hpp>

namespace dusk {

class IEventCallback
{
public:

    IEventCallback() = default;
    virtual ~IEventCallback() = default;

    virtual void Invoke(const Event&) = 0;

    inline friend bool operator==(const IEventCallback& lhs, const IEventCallback& rhs)
    {
        return lhs.IsEqualTo(rhs);
    }

    inline friend bool operator!=(const IEventCallback& lhs, const IEventCallback& rhs)
    {
        return !lhs.IsEqualTo(rhs);
    }

    // For MethodEventCallback
    inline virtual bool IsMethodOf(void * object) { return false; };

    inline virtual bool IsEqualTo(const IEventCallback& rhs) const { return false; }

}; // class IEventCallback

class FunctionEventCallback : public IEventCallback
{
public:

    FunctionEventCallback(void (*function)(const Event&))
        : _function(function)
    { }

    inline virtual void Invoke(const Event& event) override { (*_function)(event); }

    inline virtual bool IsEqualTo(const IEventCallback& rhs) const override
    {
        if (const FunctionEventCallback * convert = dynamic_cast<const FunctionEventCallback *>(&rhs))
        {
            return _function == convert->_function;
        }
        return false;
    }

private:

    void (*_function)(const Event&);

}; // class FunctionEventCallback

template <typename ObjectType = void, typename MethodType = void>
class MethodEventCallback : public IEventCallback
{
public:

    MethodEventCallback(void * object, MethodType method)
        : _object(object)
        , _method(method)
    { }

    inline virtual void Invoke(const Event& event) override
        { (static_cast<ObjectType *>(_object)->*_method)(event); }

    inline virtual bool IsMethodOf(void * object) override { return object == _object; }

    inline virtual bool IsEqualTo(const IEventCallback& rhs) const override
    {
        if (const MethodEventCallback * convert = dynamic_cast<const MethodEventCallback *>(&rhs))
        {
            return _object == convert->_object && _method == convert->_method;
        }
        return false;
    }

private:

    void * _object;
    MethodType _method;

}; // class MethodEventCallback

class LuaEventCallback : public IEventCallback
{
public:

    LuaEventCallback(lua_State * luaState, const std::string& funcName)
        : _luaState(luaState)
        , _funcName(funcName)
    { }

    virtual void Invoke(const Event& event) override;

    inline virtual bool IsEqualTo(const IEventCallback& rhs) const override
    {
        if (const LuaEventCallback * convert = dynamic_cast<const LuaEventCallback *>(&rhs))
        {
            return _luaState == convert->_luaState && _funcName == convert->_funcName;
        }
        return false;
    }

private:

    lua_State * _luaState;
    std::string _funcName;

}; // class MethodEventCallback

} // namespace dusk

#endif // DUSK_CALLBACKS_HPP
