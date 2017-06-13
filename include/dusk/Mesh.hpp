#ifndef DUSK_MESH_HPP
#define DUSK_MESH_HPP

#include <dusk/Config.hpp>
#include <dusk/Shader.hpp>
#include <dusk/Material.hpp>

namespace dusk {

struct TransformData
{
    alignas(64) glm::mat4 model = glm::mat4(1);
    alignas(64) glm::mat4 view  = glm::mat4(1);
    alignas(64) glm::mat4 proj  = glm::mat4(1);
    alignas(64) glm::mat4 mvp   = glm::mat4(1);
};

class Mesh
{
public:

    enum AttrID : GLuint
    {
        VERTS = 0,
        NORMS = 1,
        TXCDS = 2,
    };

    DISALLOW_COPY_AND_ASSIGN(Mesh);

    Mesh(Shader * shader);
    virtual ~Mesh();

    void SetBaseTransform(const glm::mat4& baseTransform);

    void SetPosition(const glm::vec3& pos);
    glm::vec3 GetPosition() const { return _position; }

    void SetRotation(const glm::vec3& rot);
    glm::vec3 GetRotation() const { return _rotation; }

    void SetScale(const glm::vec3& scale);
    glm::vec3 GetScale() const { return _scale; }

    glm::mat4 GetTransform();

    virtual bool Load();
    virtual void Free();

    virtual void Update();
    virtual void Render();

    /*
    static void InitScripting();

    static int Script_GetPosition(lua_State * L);
    static int Script_SetPosition(lua_State * L);
    static int Script_GetRotation(lua_State * L);
    static int Script_SetRotation(lua_State * L);
    static int Script_GetScale(lua_State * L);
    static int Script_SetScale(lua_State * L);
    */

protected:

    bool AddRenderGroup(Material * material,
                        GLenum drawMode,
                        const std::vector<glm::vec3>& verts,
                        const std::vector<glm::vec3>& norms,
                        const std::vector<glm::vec2>& txcds);

    bool AddRenderGroup(Material * material,
                        GLenum drawMode,
                        const std::vector<float>& verts,
                        const std::vector<float>& norms,
                        const std::vector<float>& txcds);

    bool AddRenderGroup(Material * material,
                        GLenum drawMode,
                        unsigned int vertCount,
                        const float * verts,
                        const float * norms,
                        const float * txcds);
private:

    struct RenderGroup
    {
        Material * material;

		GLsizei vertCount;

        GLenum drawMode;
        GLuint glVAO;
        GLuint glVBOs[3];
    };

    Shader * _shader;

    TransformData _shaderData;

    glm::mat4 _baseTransform;
    glm::mat4 _transform;
    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    std::vector<RenderGroup> _renderGroups;

}; // class Mesh

class FileMesh : public Mesh
{
public:

    FileMesh(Shader * shader, const std::string& filename);

    virtual bool Load() override;

private:

    std::string _filename;

    bool LoadOBJ(const std::string& filename);
    //bool LoadDMF(const std::string& filename);

}; // class FileMesh

class PlaneMesh : public Mesh
{
public:

    PlaneMesh(Shader * shader,
              Material * material,
              unsigned int rows, unsigned int cols,
              float width, float height)
        : Mesh(shader)
        , _material(material)
        , _rows(rows)
        , _cols(cols)
        , _width(width)
        , _height(height)
    { }

    virtual ~PlaneMesh()
    {
        delete _material;
    }

    virtual bool Load() override;

private:

    Material * _material;

    unsigned int _rows;
    unsigned int _cols;

    float _width;
    float _height;

};

class CuboidMesh : public Mesh
{
public:

    CuboidMesh(Shader * shader,
               Material * material,
               float width,
               float height,
               float depth)
        : Mesh(shader)
        , _material(material)
        , _width(width)
        , _height(height)
        , _depth(depth)
    { }

    virtual bool Load() override;

private:

    Material * _material;

    float _width;
    float _height;
    float _depth;

}; // class CubeMesh

class CubeMesh : public CuboidMesh
{
public:

    CubeMesh(Shader * shader, Material * material, float size)
        : CuboidMesh(shader, material, size, size, size)
    { }

}; // class CubeMesh

class UVSphereMesh : public Mesh
{
public:

    Material * _material;

    UVSphereMesh(Shader * shader,
                 Material * material,
                 unsigned int rows,
                 unsigned int cols,
                 float radius)
        : Mesh(shader)
        , _material(material)
        , _rows(rows)
        , _cols(cols)
        , _radius(radius)
    { }

    virtual bool Load() override;

private:

    unsigned int _rows;
    unsigned int _cols;

    float _radius;

}; // class UVSphereMesh

class IcoSphereMesh : public Mesh
{
public:

    IcoSphereMesh(Shader * shader,
                  Material * material,
                  unsigned int subdivisions,
                  float radius)
        : Mesh(shader)
        , _material(material)
        , _subdivisions(subdivisions)
        , _radius(radius)
    { }

    virtual bool Load() override;

private:

    Material * _material;

    unsigned int _subdivisions;

    float _radius;

}; // class IcoSphereMesh

class ConeMesh : public Mesh
{
public:

    ConeMesh(Shader * shader,
             Material * material,
             unsigned int points,
             float radius,
             float height)
        : Mesh(shader)
        , _material(material)
        , _points(points)
        , _radius(radius)
        , _height(height)
    { }

    virtual bool Load() override;

private:

    Material * _material;

    unsigned int _points;

    float _radius;
    float _height;

}; // class ConeMesh

} // namespace dusk

#endif // DUSK_MESH_HPP
