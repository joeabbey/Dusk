#ifndef DUSK_COMPONENT_HPP
#define DUSK_COMPONENT_HPP

#include <dusk/Config.hpp>

#include <dusk/Model.hpp>
#include <dusk/Camera.hpp>
#include <dusk/ScriptHost.hpp>
#include <dusk/Event.hpp>
#include <memory>

namespace dusk {

class Actor;

class Component
{
public:

    DISALLOW_COPY_AND_ASSIGN(Component);

    Component(Actor * parent);
    virtual ~Component();

    static std::unique_ptr<Component> Parse(nlohmann::json & data, Actor * actor);

    Actor * GetActor() const { return _parent; };

    static void InitScripting();
    static int Script_GetActor(lua_State * L);

protected:

    Actor * _parent;

}; // class Component

class ModelComponent : public Component
{
public:

    ModelComponent(Actor * parent, std::unique_ptr<Model> model);
    virtual ~ModelComponent();

    virtual void Update(const Event& event);
    virtual void Render(const Event& event);

    inline Model * GetModel() const { return _model.get(); };

protected:

    std::unique_ptr<Model> _model;

}; // class ModelComponent

class CameraComponent : public Component
{
public:

    CameraComponent(Actor * parent, std::unique_ptr<Camera> camera);
    virtual ~CameraComponent();

    void Update(const Event& event);
    void Render(const Event& event);

    inline Camera * GetCamera() const { return _camera.get(); };

protected:

    std::unique_ptr<Camera> _camera;

}; // class CameraComponent

class ScriptComponent : public Component
{
public:

    ScriptComponent(Actor * parent, const std::string& filename);
    virtual ~ScriptComponent() = default;

protected:

    ScriptHost _scriptHost;

    std::string _filename;

}; // class ScriptComponent

} // namespace dusk

#endif // DUSK_COMPONENT_HPP
