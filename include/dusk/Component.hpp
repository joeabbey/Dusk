#ifndef DUSK_COMPONENT_HPP
#define DUSK_COMPONENT_HPP

#include <dusk/Config.hpp>
#include <dusk/Mesh.hpp>

namespace dusk {

class Component
{
public:

    DISALLOW_COPY_AND_ASSIGN(Component);

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

    MeshComponent(const std::string& filename, Shader * shader);
    virtual ~MeshComponent();

    bool Load() override;
    void Free() override;

    void Update() override;
    void Render() override;

protected:

    TransformData _shaderData;

    Mesh * _mesh;
    Shader * _shader;
    std::string _filename;

}; // class MeshComponent

class ScriptComponent : public Component
{
public:

    ScriptComponent(const std::string& filename);
    virtual ~ScriptComponent();

    bool Load() override;
    void Free() override;

    void Update() override;
    void Render() override;

protected:

    std::string _filename;

}; // class ScriptComponent

} // namespace dusk

#endif // DUSK_COMPONENT_HPP
