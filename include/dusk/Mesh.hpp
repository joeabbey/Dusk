#ifndef DUSK_MESH_HPP
#define DUSK_MESH_HPP

#include <dusk/Config.hpp>

#include <dusk/App.hpp>
#include <dusk/Shader.hpp>
#include <dusk/Material.hpp>
#include <memory>
#include <sstream>

namespace dusk {

class Mesh
{
public:

    struct Data
    {
        std::vector<glm::vec3> Vertices;
        std::vector<glm::vec3> Normals;
        std::vector<glm::vec2> TexCoords;

        std::vector<unsigned int> Indices = std::vector<unsigned int>();

        std::shared_ptr<dusk::Material> Material = nullptr;

        GLenum DrawMode = GL_TRIANGLES;
    };

    /// Class Boilerplate

    DISALLOW_COPY_AND_ASSIGN(Mesh);

    static void LuaSetup(sol::state& lua);

    Mesh() = default;

    Mesh(Mesh&& rhs)
    {
        std::swap(_loaded, rhs._loaded);
        std::swap(_glVAO, rhs._glVAO);
        memcpy(_glVBOs, rhs._glVBOs, sizeof(_glVBOs));
        memset(rhs._glVBOs, 0, sizeof(rhs._glVBOs));
        _renderGroups = std::move(rhs._renderGroups);
    }

    Mesh(const std::string& filename)
    {
        LoadFromFile(filename);
    }

    Mesh(const Data& data)
    {
        LoadFromData(data);
    }

    virtual ~Mesh();

    void Serialize(nlohmann::json& data);
    void Deserialize(nlohmann::json& data);

    bool LoadFromFile(const std::string& filename);

    bool LoadFromData(const Data& data);

    bool IsLoaded() const { return _loaded; }

    Box GetBounds() const { return _bounds; }

    void Render(RenderContext& ctx);

private:

    struct RenderGroup
    {
        GLenum drawMode;
        std::shared_ptr<Material> material;
        unsigned int start;
        unsigned int count;
    };

    Box ComputeBounds(const std::vector<glm::vec3>& verts);

    bool _loaded = false;

    GLuint _glVAO;
    GLuint _glVBOs[3];

    Box _bounds;

    std::vector<RenderGroup> _renderGroups;
};

} // namespace dusk

#endif // DUSK_MESH_HPP
