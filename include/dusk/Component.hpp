#ifndef DUSK_COMPONENT_HPP
#define DUSK_COMPONENT_HPP

#include <dusk/Config.hpp>
#include <dusk/Mesh.hpp>
#include <dusk/Camera.hpp>
#include <dusk/ScriptHost.hpp>
#include <dusk/Event.hpp>

namespace dusk {

class Actor;

class Component
{
public:

    DISALLOW_COPY_AND_ASSIGN(Component);

    Component(Actor * parent);
    virtual ~Component();

    Actor * GetActor() const { return _parent; };

    bool IsLoaded() const { return _loaded; }

    virtual bool Load() = 0;
    virtual void Free() = 0;

    static void InitScripting();
    static int Script_GetActor(lua_State * L);

protected:

    bool _loaded;

    Actor * _parent;

}; // class Component

class MeshComponent : public Component
{
public:

    MeshComponent(Actor * parent, Mesh * mesh);
    virtual ~MeshComponent();

    bool Load() override;
    void Free() override;

    void Update(const Event& event);
    void Render(const Event& event);

    inline Mesh * GetMesh() const { return _mesh; };

protected:

    Mesh * _mesh;

}; // class MeshComponent

class CameraComponent : public Component
{
public:

    CameraComponent(Actor * parent, Camera * camera);
    virtual ~CameraComponent();

    bool Load() override;
    void Free() override;

    void Update(const Event& event);
    void Render(const Event& event);

    inline Camera * GetCamera() const { return _camera; };

protected:

    Camera * _camera;

}; // class CameraComponent

class ScriptComponent : public Component
{
public:

    ScriptComponent(Actor * parent, const std::string& filename);
    virtual ~ScriptComponent();

    bool Load() override;
    void Free() override;

protected:

    ScriptHost _scriptHost;

    std::string _filename;

}; // class ScriptComponent

} // namespace dusk

#endif // DUSK_COMPONENT_HPP
