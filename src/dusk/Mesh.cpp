#include "dusk/Mesh.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>

namespace dusk {

void Mesh::LuaSetup(sol::state& lua)
{
    /*
    lua.new_usertype<Mesh>("Mesh",
        "new", sol::constructors<Mesh(), Mesh(const std::string& filename)>(),
        "LoadFromFile", &Mesh::LoadFromFile,
        "IsLoaded", &Mesh::IsLoaded,
        "Render", &Mesh::Render
    );
    */
}

Mesh::~Mesh()
{
    glDeleteBuffers(sizeof(_glVBOs) / sizeof(GLuint), _glVBOs);
    glDeleteVertexArrays(1, &_glVAO);
}

void Mesh::Serialize(nlohmann::json& data)
{

}

void Mesh::Deserialize(nlohmann::json& data)
{

}

bool Mesh::LoadFromFile(const std::string& filename)
{
    DuskBenchStart();

    bool ret;
    std::string err;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string dirname = GetDirname(filename) + "/";

    DuskLogLoad("Loading model from '%s'", filename.c_str());

    _loaded = false;

    if (filename.empty())
    {
        return false;
    }

    ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), dirname.c_str());
    if (!ret)
    {
        DuskLogError("Failed to load '%s': %s\n", filename.c_str(), err.c_str());
        return false;
    }

    glGenVertexArrays(1, &_glVAO);
    glBindVertexArray(_glVAO);

    glGenBuffers(sizeof(_glVBOs) / sizeof(GLuint), _glVBOs);

    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> norms;
    std::vector<glm::vec2> txcds;

    bool hasNorms = !attrib.normals.empty();
    bool hasTxcds = !attrib.texcoords.empty();

    unsigned int offset = 0;
    for (tinyobj::shape_t & shape : shapes)
    {
        DuskLogVerbose("Processing shape %s, offset %u", shape.name.c_str(), offset);

        tinyobj::mesh_t & mesh = shape.mesh;
        std::shared_ptr<Material> material;

        if (!mesh.material_ids.empty() && mesh.material_ids[0] >= 0)
        {
            tinyobj::material_t & mat = materials[mesh.material_ids[0]];

            std::string ambientFilename = (mat.ambient_texname.empty()
                ? std::string()
                : dirname + mat.ambient_texname);
            std::string diffuseFilename = (mat.diffuse_texname.empty()
                ? std::string()
                : dirname + mat.diffuse_texname);
            std::string specularFilename = (mat.specular_texname.empty()
                ? std::string()
                : dirname + mat.specular_texname);
            std::string bumpFilename = (mat.bump_texname.empty()
                ? std::string()
                : dirname + mat.bump_texname);

            material.reset(new Material({
                glm::vec4(mat.ambient[0], mat.ambient[1], mat.ambient[2], 1.0f),
                glm::vec4(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1.0f),
                glm::vec4(mat.specular[0], mat.specular[1], mat.specular[2], 1.0f),
                mat.shininess, mat.dissolve,
                ambientFilename,
                diffuseFilename,
                specularFilename,
                bumpFilename
            }));
        }

        if (mesh.num_face_vertices.empty())
        {
            // ???
            continue;
        }

        verts.resize(verts.size() + mesh.indices.size());
        if (hasNorms)
        {
            norms.resize(norms.size() + mesh.indices.size());
        }
        if (hasTxcds)
        {
            txcds.resize(txcds.size() + mesh.indices.size());
        }

        GLenum drawMode = GL_TRIANGLES;
        if (mesh.num_face_vertices.front() == 4)
        {
            // TODO: Cleanup?
            DuskLogError("Loading Mesh with Quads");
            return false;
        }

        size_t index = 0;
        for (size_t f = 0; f < mesh.num_face_vertices.size(); ++f)
        {
           size_t fv = mesh.num_face_vertices[f];

           for (size_t v = 0; v < fv; ++v)
           {
               tinyobj::index_t idx = mesh.indices[index + v];

               verts[offset + index + v] = {
                   attrib.vertices[3 * idx.vertex_index + 0],
                   attrib.vertices[3 * idx.vertex_index + 1],
                   attrib.vertices[3 * idx.vertex_index + 2],
               };

               if (hasNorms)
               {
                   norms[offset + index + v] = {
                       attrib.normals[3 * idx.normal_index + 0],
                       attrib.normals[3 * idx.normal_index + 1],
                       attrib.normals[3 * idx.normal_index + 2],
                   };
               }

               if (hasTxcds)
               {
                   txcds[offset + index + v] = {
                       attrib.texcoords[2 * idx.texcoord_index + 0],
                       attrib.texcoords[2 * idx.texcoord_index + 1],
                   };
               }
           }
           index += fv;
        }

        _renderGroups.push_back({
            drawMode,
            std::move(material),
            offset,
            (unsigned int) mesh.indices.size()
        });
        offset += mesh.indices.size();
    }

    _bounds = ComputeBounds(verts);

    glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), (GLfloat *)verts.data(), GL_STATIC_DRAW);

    if (norms.empty())
    {
        glDeleteBuffers(1, &_glVBOs[1]);
        _glVBOs[1] = 0;
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * norms.size(), (GLfloat *)norms.data(), GL_STATIC_DRAW);
    }

    if (txcds.empty())
    {
        glDeleteBuffers(1, &_glVBOs[2]);
        _glVBOs[2] = 0;
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * txcds.size(), (GLfloat *)txcds.data(), GL_STATIC_DRAW);
    }

    DuskBenchEnd("Mesh::LoadFromFile");
    _loaded = true;
    return true;
}

bool Mesh::LoadFromData(const Data& data)
{
    DuskBenchStart();
    _loaded = false;

    if (data.Vertices.empty())
    {
        DuskLogWarn("Cannot add empty mesh");
        return false;
    }

    _bounds = ComputeBounds(data.Vertices);

    DuskLogLoad("Loading Mesh to OpenGL");

    _renderGroups.push_back({
        data.DrawMode, data.Material, 0, (unsigned int) data.Vertices.size()
    });

    glGenVertexArrays(1, &_glVAO);
    glBindVertexArray(_glVAO);

    glGenBuffers(sizeof(_glVBOs) / sizeof(GLuint), _glVBOs);

    glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.Vertices.size(), (GLfloat *) data.Vertices.data(), GL_STATIC_DRAW);

    if (data.Normals.empty())
    {
        glDeleteBuffers(1, &_glVBOs[1]);
        _glVBOs[1] = 0;
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.Normals.size(), (GLfloat *) data.Normals.data(), GL_STATIC_DRAW);
    }

    if (data.TexCoords.empty())
    {
        glDeleteBuffers(1, &_glVBOs[2]);
        _glVBOs[2] = 0;
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * data.TexCoords.size(), (GLfloat *) data.TexCoords.data(), GL_STATIC_DRAW);
    }

    DuskLogVerbose("Bound mesh to VAO %u", _glVAO);

    // TODO: Colors?

    DuskBenchEnd("Mesh::LoadFromData");
    _loaded = true;
    return true;
}

void Mesh::Render(RenderContext& ctx)
{
    if (!ctx.CurrentShader) return;

    ctx.CurrentShader->Bind();

    glBindVertexArray(_glVAO);

    glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[0]);
    glVertexAttribPointer(ctx.CurrentShader->GetAttributeLocation("_Position"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(ctx.CurrentShader->GetAttributeLocation("_Position"));

    if (_glVBOs[1] > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[1]);
        glVertexAttribPointer(ctx.CurrentShader->GetAttributeLocation("_Normal"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(ctx.CurrentShader->GetAttributeLocation("_Normal"));
    }

    if (_glVBOs[2] > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[2]);
        glVertexAttribPointer(ctx.CurrentShader->GetAttributeLocation("_TexCoord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(ctx.CurrentShader->GetAttributeLocation("_TexCoord"));
    }

    for (const RenderGroup& group : _renderGroups)
    {
        if (group.material)
        {
            group.material->Bind(ctx.CurrentShader);
        }

        glDrawArrays(group.drawMode, group.start, group.count);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Box Mesh::ComputeBounds(const std::vector<glm::vec3>& verts)
{
    Box bounds;

    for (const glm::vec3& v : verts)
    {
        bounds.Min = glm::min(bounds.Min, v);
        bounds.Max = glm::max(bounds.Max, v);
    }

    return bounds;
}

} // namespace dusk
