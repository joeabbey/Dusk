#ifndef DUSK_COMPONENT_HPP
#define DUSK_COMPONENT_HPP

#include <dusk/Config.hpp>

#include <dusk/Model.hpp>
#include <dusk/Camera.hpp>
#include <memory>

namespace dusk {

class Actor;

class ActorComponent
{
public:

    DISALLOW_COPY_AND_ASSIGN(Component);

    Component();
    virtual ~Component();

    virtual void SetActor(Actor * actor);
    Actor * GetActor() const { return _actor; };

protected:

    Actor * _actor;

}; // class Component

class ModelComponent : public ActorComponent
{
public:

    ModelComponent(std::unique_ptr<Model> model);
    virtual ~ModelComponent();

    virtual void SetActor(Actor * actor) override;

    virtual void OnUpdate(const UpdateContext& event);
    virtual void OnRender(RenderContext& event);

    inline Model * GetModel() const { return _model.get(); };

protected:

    unsigned int updateBindId = 0;
    unsigned int renderBindId = 0;

    std::unique_ptr<Model> _model;

}; // class ModelComponent

class CameraComponent : public ActorComponent
{
public:

    CameraComponent(std::unique_ptr<Camera> camera);
    virtual ~CameraComponent();

    virtual void SetActor(Actor * actor) override;

    inline Camera * GetCamera() const { return _camera.get(); };

protected:

    std::unique_ptr<Camera> _camera;

}; // class CameraComponent

//class ScriptComponent : public ActorComponent
//{
//public:
//
//    ScriptComponent(const std::string& filename, bool isTemplate = false);
//    virtual ~ScriptComponent() = default;
//
//    virtual std::unique_ptr<Component> Clone() override;
//
//    virtual void SetActor(Actor * actor) override;
//
//protected:
//
//    ScriptHost _scriptHost;
//
//    std::string _filename;
//
//}; // class ScriptComponent

} // namespace dusk

#endif // DUSK_COMPONENT_HPP
