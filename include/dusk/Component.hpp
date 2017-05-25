#ifndef DUSK_COMPONENT_HPP
#define DUSK_COMPONENT_HPP

#include <dusk/Config.hpp>
#include <dusk/Mesh.hpp>
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

protected:

    bool _loaded;

    Actor * _parent;

}; // class Component

struct TransformData
{
    alignas(64) glm::mat4 model = glm::mat4(1);
    alignas(64) glm::mat4 view  = glm::mat4(1);
    alignas(64) glm::mat4 proj  = glm::mat4(1);
    alignas(64) glm::mat4 mvp   = glm::mat4(1);
};

class MeshComponent : public Component
{
public:

    MeshComponent(Actor * parent, const std::string& filename, Shader * shader);
    virtual ~MeshComponent();

    bool Load() override;
    void Free() override;

    void Update(const Event& event);
    void Render(const Event& event);

protected:

    TransformData _shaderData;

    Mesh * _mesh;
    Shader * _shader;
    std::string _filename;

    glm::vec3   _position;
    glm::vec3   _rotation;
    glm::vec3   _scale;
    glm::mat4   _transform;

}; // class MeshComponent

class ScriptComponent : public Component
{
public:

    ScriptComponent(Actor * parent, const std::string& filename);
    virtual ~ScriptComponent();

    bool Load() override;
    void Free() override;

protected:

    std::string _filename;

}; // class ScriptComponent

} // namespace dusk

#endif // DUSK_COMPONENT_HPP
