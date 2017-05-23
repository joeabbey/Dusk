#ifndef DUSK_MESH_HPP
#define DUSK_MESH_HPP

#include <dusk/Config.hpp>
//#include <dusk/Material.hpp>

namespace dusk {

struct Material { };

class Mesh
{
public:

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh();
    Mesh(const std::string& name);
    virtual ~Mesh();

    std::string GetName() const { return _name; }

    bool Load(const std::string file);
    void Free();

private:

    struct RenderGroup {
        Material * material;
        GLuint glVAO;
    };

    std::string _name;

    std::vector<RenderGroup> _renderGroups;

}; // class Mesh

} // namespace dusk

#endif // DUSK_MESH_HPP
