#ifndef DUSK_MESH_HPP
#define DUSK_MESH_HPP

#include <dusk/Config.hpp>
#include <dusk/Shader.hpp>
#include <dusk/Material.hpp>

namespace dusk {

class Mesh
{
public:

    enum AttrID : GLuint
    {
        VERTS = 0,
        NORMS = 1,
        TXCDS = 2,
    };

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(const std::string& filename);
    virtual ~Mesh();

    bool Load();
    void Free();

    void Update();
    void Render();

private:

    struct RenderGroup
    {
        Material * material;

        unsigned int vertCount;

        GLenum drawMode;
        GLuint glVAO;
        GLuint glVBOs[3];
    };

    bool LoadOBJ(const std::string filename);
    //bool LoadDMF(const std::string filename);

    static bool LoadVAO(RenderGroup * group,
                          const std::vector<float>& verts,
                          const std::vector<float>& norms,
                          const std::vector<float>& txcds);

    std::string _filename;
    std::vector<RenderGroup> _renderGroups;

}; // class Mesh

} // namespace dusk

#endif // DUSK_MESH_HPP
