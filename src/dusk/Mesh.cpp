#include "dusk/Mesh.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>
#include <dusk/App.hpp>
#include <dusk/Camera.hpp>

namespace dusk {

Mesh::Mesh(Shader * shader)
    : _shader(shader)
    , _baseTransform(1)
    , _transform(1)
    , _position(0)
    , _rotation(0)
    , _scale(1)
    , _renderGroups()
{ }

Mesh::~Mesh()
{
    Free();
}

void Mesh::SetBaseTransform(const glm::mat4& baseTransform)
{
    _baseTransform = baseTransform;
}

void Mesh::SetPosition(const glm::vec3& pos)
{
    _position = pos;
}

void Mesh::SetRotation(const glm::vec3& rot)
{
    _rotation = rot;
}

void Mesh::SetScale(const glm::vec3& scale)
{
    _scale = scale;
}

glm::mat4 Mesh::GetTransform()
{
    _transform = _baseTransform;
    _transform = glm::scale(_transform, _scale);
    _transform = glm::rotate(_transform, _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    _transform = glm::rotate(_transform, _rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    _transform = glm::translate(_transform, _position);

    return _transform;
}

bool Mesh::Load()
{
    DuskBenchStart();

    bool ret = true;

    for (RenderGroup& group : _renderGroups)
    {
        if (group.material)
        {
            ret &= group.material->Load(_shader);
        }
    }

    Shader::AddData("DuskTransformData", &_shaderData, sizeof(_shaderData));
    _shader->BindData("DuskTransformData");

    DuskBenchEnd("Mesh::Load()");
    return ret;
}

void Mesh::Free()
{
    for (RenderGroup& group : _renderGroups)
    {
        if (group.material)
        {
            group.material->Free();
        }
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
    _shader->Bind();

    // TODO: Move camera somewhere else
    Camera * camera = App::GetInst()->GetScene()->GetCamera();

    _shaderData.model = GetTransform();
    _shaderData.view = camera->GetView();
    _shaderData.proj = camera->GetProjection();
    _shaderData.mvp = _shaderData.proj * _shaderData.view * _shaderData.model;

    Shader::UpdateData("DuskTransformData", &_shaderData, sizeof(_shaderData));

    for (RenderGroup& group : _renderGroups)
    {
        if (group.material)
        {
            group.material->Bind(_shader);
        }

        glBindVertexArray(group.glVAO);
        glDrawArrays(group.drawMode, 0, group.vertCount);
    }
    glBindVertexArray(0);
}

bool Mesh::AddRenderGroup(Material * material,
                          GLenum drawMode,
                          const std::vector<float>& verts,
                          const std::vector<float>& norms,
                          const std::vector<float>& txcds)
{
    RenderGroup group;
    group.vertCount = (unsigned int)verts.size();
    group.material = material;
    group.drawMode = drawMode;

    glGenVertexArrays(1, &group.glVAO);
    glBindVertexArray(group.glVAO);

    glGenBuffers(3, group.glVBOs);

    glBindBuffer(GL_ARRAY_BUFFER, group.glVBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts.size(), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(Mesh::AttrID::VERTS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(Mesh::AttrID::VERTS);

    if (!norms.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, group.glVBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * norms.size(), norms.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(Mesh::AttrID::NORMS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(Mesh::AttrID::NORMS);
    }

    if (!txcds.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, group.glVBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * txcds.size(), txcds.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(Mesh::AttrID::TXCDS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(Mesh::AttrID::TXCDS);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    _renderGroups.push_back(group);
    return true;
}

bool FileMesh::Load()
{
    DuskBenchStart();
    DuskLogInfo("Loading model from '%s'", _filename.c_str());

    bool ret = true;

    std::string ext = GetExtension(_filename);
    if (ext == "obj")
    {
        ret &= LoadOBJ(_filename);
    }
    else if (ext == "dmf" || ext == "dmfz")
    {
        //ret = LoadDMF(_filename);
    }

    ret &= Mesh::Load();

    DuskBenchEnd("FileMesh::Load()");
    return ret;
}

FileMesh::FileMesh(Shader * shader, const std::string& filename)
    : Mesh(shader)
    , _filename(filename)
{
}

bool FileMesh::LoadOBJ(const std::string& filename)
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
        tinyobj::mesh_t &     mesh = shape.mesh;
        tinyobj::material_t * mat  = nullptr;

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

        Material * material = nullptr;
        if (mat)
        {
            std::string ambient_texname = (mat->ambient_texname.empty()
                ? std::string()
                : dirname + mat->ambient_texname);
            std::string diffuse_texname = (mat->diffuse_texname.empty()
                ? std::string()
                : dirname + mat->diffuse_texname);
            std::string specular_texname = (mat->specular_texname.empty()
                ? std::string()
                : dirname + mat->specular_texname);
            std::string bump_texname = (mat->bump_texname.empty()
                ? std::string()
                : dirname + mat->bump_texname);

            material = new Material(
                glm::vec4(mat->ambient[0], mat->ambient[1], mat->ambient[2], 1.0f),
                glm::vec4(mat->diffuse[0], mat->diffuse[1], mat->diffuse[2], 1.0f),
                glm::vec4(mat->specular[0], mat->specular[1], mat->specular[2], 1.0f),
                mat->shininess, mat->dissolve,
                ambient_texname,
                diffuse_texname,
                specular_texname,
                bump_texname
            );
        }
        AddRenderGroup(material, GL_TRIANGLES, verts, norms, txcds);
    }

    return true;
}

} // namespace dusk
