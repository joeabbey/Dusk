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

    Component(bool isTemplate = false);
    virtual ~Component();

    static std::unique_ptr<Component> Parse(nlohmann::json & data, bool isTemplate = false);

    virtual std::unique_ptr<Component> Clone();

    inline bool IsTemplate() const { return _isTemplate; }

    virtual void SetActor(Actor * actor);
    Actor * GetActor() const { return _actor; };

    static void InitScripting();
    static int Script_GetActor(lua_State * L);

protected:

    Actor * _actor;

    bool _isTemplate;

}; // class Component

class ModelComponent : public Component
{
public:

    ModelComponent(std::unique_ptr<Model> model, bool isTemplate = false);
    virtual ~ModelComponent();

    virtual std::unique_ptr<Component> Clone() override;

    virtual void SetActor(Actor * actor) override;

    virtual void OnUpdate(const Event& event);
    virtual void OnRender(const Event& event);

    inline Model * GetModel() const { return _model.get(); };

protected:

    std::unique_ptr<Model> _model;

}; // class ModelComponent

class CameraComponent : public Component
{
public:

    CameraComponent(std::unique_ptr<Camera> camera, bool isTemplate = false);
    virtual ~CameraComponent();

    virtual std::unique_ptr<Component> Clone() override;

    virtual void SetActor(Actor * actor) override;

    void OnUpdate(const Event& event);

    inline Camera * GetCamera() const { return _camera.get(); };

protected:

    std::unique_ptr<Camera> _camera;

}; // class CameraComponent

class ScriptComponent : public Component
{
public:

    ScriptComponent(const std::string& filename, bool isTemplate = false);
    virtual ~ScriptComponent() = default;

    virtual std::unique_ptr<Component> Clone() override;

    virtual void SetActor(Actor * actor) override;

protected:

    ScriptHost _scriptHost;

    std::string _filename;

}; // class ScriptComponent

} // namespace dusk

#endif // DUSK_COMPONENT_HPP
