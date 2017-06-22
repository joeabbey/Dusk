#ifndef DUSK_MESH_HPP
#define DUSK_MESH_HPP

#include <dusk/Config.hpp>

#include <dusk/EventDispatcher.hpp>
#include <dusk/Shader.hpp>
#include <dusk/Material.hpp>
#include <memory>
#include <sstream>

namespace dusk {

class Mesh
    : public std::enable_shared_from_this<Mesh>
    , public IEventDispatcher
{
public:

    enum AttrID : GLuint
    {
        VERTS = 0,
        NORMS = 1,
        TXCDS = 2,
    };

    DISALLOW_COPY_AND_ASSIGN(Mesh);

    virtual ~Mesh();

    static std::shared_ptr<Mesh> Parse(nlohmann::json & data);

    virtual void Update();
    virtual void Render(Shader * shader);

protected:

    Mesh();

    bool AddRenderGroup(std::shared_ptr<Material> material,
                        GLenum drawMode,
                        const std::vector<glm::vec3>& verts,
                        const std::vector<glm::vec3>& norms,
                        const std::vector<glm::vec2>& txcds);

    bool AddRenderGroup(std::shared_ptr<Material> material,
                        GLenum drawMode,
                        const std::vector<float>& verts,
                        const std::vector<float>& norms,
                        const std::vector<float>& txcds);

    bool AddRenderGroup(std::shared_ptr<Material> material,
                        GLenum drawMode,
                        unsigned int vertCount,
                        const float * verts,
                        const float * norms,
                        const float * txcds);
private:

    struct RenderGroup
    {
        std::shared_ptr<Material> material;

		GLsizei vertCount;

        GLenum drawMode;
        GLuint glVAO;
        GLuint glVBOs[3];
    };

    std::vector<RenderGroup> _renderGroups;

}; // class Mesh

class FileMesh : public Mesh
{
public:

    static std::shared_ptr<FileMesh>
    Create(const std::string& filename);

protected:

    FileMesh(const std::string& filename);

private:

    std::string _filename;

    bool LoadOBJ(const std::string& filename);
    //bool LoadDMF(const std::string& filename);

}; // class FileMesh

class PlaneMesh : public Mesh
{
public:

    static std::shared_ptr<PlaneMesh>
    Create(std::shared_ptr<Material> material,
           unsigned int rows, unsigned int cols,
           float width, float height);

protected:

    PlaneMesh(std::shared_ptr<Material> material,
              unsigned int rows, unsigned int cols,
              float width, float height);

    std::shared_ptr<Material> _material;

private:

    unsigned int _rows;
    unsigned int _cols;

    float _width;
    float _height;

};

class CuboidMesh : public Mesh
{
public:

    static std::shared_ptr<CuboidMesh>
    Create(std::shared_ptr<Material> material,
           float width,
           float height,
           float depth);

protected:

    CuboidMesh(std::shared_ptr<Material> material,
        float width,
        float height,
        float depth);

private:

    std::shared_ptr<Material> _material;

    float _width;
    float _height;
    float _depth;

}; // class CubeMesh

class CubeMesh : public CuboidMesh
{
public:

    static std::shared_ptr<CubeMesh>
    Create(std::shared_ptr<Material> material, float size);

protected:

    CubeMesh(std::shared_ptr<Material> material, float size)
        : CuboidMesh(material, size, size, size)
    { }

}; // class CubeMesh

class UVSphereMesh : public Mesh
{
public:

    static std::shared_ptr<UVSphereMesh>
    Create(std::shared_ptr<Material> material,
           unsigned int rows,
           unsigned int cols,
           float radius);

protected:

    UVSphereMesh(std::shared_ptr<Material> material,
                 unsigned int rows,
                 unsigned int cols,
                 float radius)
        : Mesh()
        , _material(material)
        , _rows(rows)
        , _cols(cols)
        , _radius(radius)
    {
        (void)_material;
        (void)_rows;
        (void)_cols;
        (void)_radius;
    }

private:

    std::shared_ptr<Material> _material;

    unsigned int _rows;
    unsigned int _cols;

    float _radius;

}; // class UVSphereMesh

class IcoSphereMesh : public Mesh
{
public:

    static std::shared_ptr<IcoSphereMesh>
    Create(std::shared_ptr<Material> material,
           unsigned int subdivisions,
           float radius);

protected:

    IcoSphereMesh(std::shared_ptr<Material> material,
                  unsigned int subdivisions,
                  float radius)
        : Mesh()
        , _material(material)
        , _subdivisions(subdivisions)
        , _radius(radius)
    {
        (void)_material;
        (void)_subdivisions;
        (void)_radius;
    }

private:

    std::shared_ptr<Material> _material;

    unsigned int _subdivisions;

    float _radius;

}; // class IcoSphereMesh

class ConeMesh : public Mesh
{
public:

    static std::shared_ptr<ConeMesh>
    Create(std::shared_ptr<Material> material,
           unsigned int points,
           float radius,
           float height);

protected:

    ConeMesh(std::shared_ptr<Material> material,
             unsigned int points,
             float radius,
             float height);

private:

    std::shared_ptr<Material> _material;

    unsigned int _points;

    float _radius;
    float _height;

}; // class ConeMesh

} // namespace dusk

#endif // DUSK_MESH_HPP
