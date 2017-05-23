#ifndef DUSK_COMPONENT_HPP
#define DUSK_COMPONENT_HPP

#include <dusk/Config.hpp>
#include <dusk/Mesh.hpp>

namespace dusk {

class Component
{
public:

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    Component();
    virtual ~Component();

    bool IsLoaded() const { return _loaded; }

    virtual bool Load() = 0;
    virtual void Free() = 0;

    virtual void Update() = 0;
    virtual void Render() = 0;

protected:

    bool _loaded;

}; // class Component

class MeshComponent : public Component
{
public:

    MeshComponent(const std::string& file);
    virtual ~MeshComponent();

    bool Load() override;
    void Free() override;

    void Update() override;
    void Render() override;

protected:

    Mesh * _mesh;
    std::string _file;

}; // class MeshComponent

class ScriptComponent : public Component
{
public:

    ScriptComponent(const std::string& file);
    virtual ~ScriptComponent();

    bool Load() override;
    void Free() override;

    void Update() override;
    void Render() override;

protected:

    std::string _file;

}; // class ScriptComponent

} // namespace dusk

#endif // DUSK_COMPONENT_HPP
