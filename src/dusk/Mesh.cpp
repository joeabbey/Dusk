#include "dusk/Mesh.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>

namespace dusk {

Mesh::Mesh(const std::string& filename)
    : _filename(filename)
    , _renderGroups()
{ }

Mesh::~Mesh()
{
    Free();
}

bool Mesh::Load()
{
    DuskBenchStart();
    DuskLogInfo("Loading mesh '%s'", _filename.c_str());

    bool ret = false;

    std::string ext = GetExtension(_filename);
    if (ext == "obj")
    {
        ret = LoadOBJ(_filename);
    }
    else if (ext == "dmf" || ext == "dmfz")
    {
        //ret = LoadDMF(_filename);
    }

    DuskBenchEnd("Mesh::Load()");
    return ret;
}

void Mesh::Free()
{
    for (RenderGroup& group : _renderGroups)
    {
        delete group.material;
        glDeleteVertexArrays(1, &group.glVAO);
        glDeleteBuffers(3, group.glVBOs);
    }
    _renderGroups.clear();
}

void Mesh::Update()
{

}

void Mesh::Render()
{
    for (RenderGroup& group : _renderGroups)
    {
        if (NULL != group.material)
            group.material->Bind();

        glBindVertexArray(group.glVAO);
        glDrawArrays(group.drawMode, 0, group.vertCount);
    }
    glBindVertexArray(0);
}

bool Mesh::LoadOBJ(const std::string filename)
{
    bool                        ret;
    std::string                 err;
    tinyobj::attrib_t           attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;

    std::string dirname = GetDirname(filename) + "/";

    ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), dirname.c_str());

    if (!ret)
    {
        DuskLogError("Failed to load %s: %s\n", filename.c_str(), err.c_str());
        return false;
    }

    bool has_norms = !attrib.normals.empty();
    bool has_txcds = !attrib.texcoords.empty();

    for (tinyobj::shape_t & shape : shapes)
    {
        _renderGroups.push_back(RenderGroup());
        RenderGroup & group = _renderGroups.back();

        tinyobj::mesh_t &     mesh = shape.mesh;
        tinyobj::material_t * mat  = NULL;

        if (!mesh.material_ids.empty() && mesh.material_ids[0] >= 0)
        {
            mat = &materials[mesh.material_ids[0]];
        }

        std::vector<float> verts;
        std::vector<float> norms;
        std::vector<float> txcds;

        verts.resize(mesh.indices.size() * 3);

        if (has_norms)
        {
            norms.resize(mesh.indices.size() * 3);
        }

        if (has_txcds)
        {
            txcds.resize(mesh.indices.size() * 2);
        }

        size_t index = 0;
        for (size_t f = 0; f < mesh.num_face_vertices.size(); ++f)
        {
            size_t fv = mesh.num_face_vertices[f];

            for (size_t v = 0; v < fv; ++v)
            {
                tinyobj::index_t idx = mesh.indices[index + v];

                verts[(index + v) * 3 + 0] = attrib.vertices[3 * idx.vertex_index + 0];
                verts[(index + v) * 3 + 1] = attrib.vertices[3 * idx.vertex_index + 1];
                verts[(index + v) * 3 + 2] = attrib.vertices[3 * idx.vertex_index + 2];

                if (has_norms)
                {
                    norms[(index + v) * 3 + 0] = attrib.normals[3 * idx.normal_index + 0];
                    norms[(index + v) * 3 + 1] = attrib.normals[3 * idx.normal_index + 1];
                    norms[(index + v) * 3 + 2] = attrib.normals[3 * idx.normal_index + 2];
                }

                if (has_txcds)
                {
                    txcds[(index + v) * 2 + 0] = attrib.texcoords[2 * idx.texcoord_index + 0];
                    txcds[(index + v) * 2 + 1] = attrib.texcoords[2 * idx.texcoord_index + 1];
                }
            }
            index += fv;
        }

        group.material = nullptr;
        group.vertCount = verts.size();
        group.drawMode = GL_TRIANGLES;
        LoadVAO(&group, verts, norms, txcds);
    }

    return true;
}

bool Mesh::LoadVAO(RenderGroup * group,
               const std::vector<float>& verts,
               const std::vector<float>& norms,
               const std::vector<float>& txcds)
{

    glGenVertexArrays(1, &group->glVAO);
    glBindVertexArray(group->glVAO);

    glGenBuffers(3, group->glVBOs);

    glBindBuffer(GL_ARRAY_BUFFER, group->glVBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts.size(), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(Mesh::AttrID::VERTS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(Mesh::AttrID::VERTS);

    if (!norms.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, group->glVBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * norms.size(), norms.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(Mesh::AttrID::NORMS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(Mesh::AttrID::NORMS);
    }

    if (!txcds.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, group->glVBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * txcds.size(), txcds.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(Mesh::AttrID::TXCDS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(Mesh::AttrID::TXCDS);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

} // namespace dusk
