#include "dusk/Mesh.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>
#include <dusk/App.hpp>
#include <dusk/Asset.hpp>

namespace dusk {

Mesh::Mesh()
    : _renderGroups()
{
}

Mesh::~Mesh()
{
}

std::shared_ptr<Mesh> Mesh::Parse(nlohmann::json & data)
{
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material(nullptr);

    const std::string& type = data["Type"];

    // TODO: Infer type

    if (type == "File")
    {
    	mesh = FileMesh::Create(data["File"].get<std::string>());
    }
    else if (type == "Plane")
    {
        mesh = PlaneMesh::Create(material, data["Rows"], data["Cols"],
                                           data["Width"], data["Height"]);
    }
    else if (type == "Cuboid")
    {
        mesh = CuboidMesh::Create(material, data["Width"], data["Height"], data["Depth"]);
    }
    else if (type == "Cube")
    {
        mesh = CubeMesh::Create(material, data["Size"]);
    }
    else if (type == "UVSphere")
    {
        mesh = UVSphereMesh::Create(material, data["Rows"], data["Cols"], data["Radius"]);
    }
    else if (type == "IcoSphere")
    {
        mesh = IcoSphereMesh::Create(material, data["subdivisions"], data["Radius"]);
    }
    else if (type == "Cone")
    {
    	mesh = ConeMesh::Create(material, data["Points"], data["Radius"], data["Height"]);
    }

    return mesh;
}

void Mesh::Update()
{
}

void Mesh::Render(Shader * shader)
{
    for (RenderGroup& group : _renderGroups)
    {
        if (group.material)
        {
            group.material->Bind(shader);
        }

        glBindVertexArray(group.glVAO);
        glDrawArrays(group.drawMode, 0, group.vertCount);
    }
    glBindVertexArray(0);
}

bool Mesh::AddRenderGroup(std::shared_ptr<Material> material,
                          GLenum drawMode,
                          const std::vector<glm::vec3>& verts,
                          const std::vector<glm::vec3>& norms,
                          const std::vector<glm::vec2>& txcds)
{
    return AddRenderGroup(material, drawMode, (unsigned int)verts.size(),
                         (float *)verts.data(),
                         (norms.empty() ? nullptr : (float *)norms.data()),
                         (txcds.empty() ? nullptr : (float *)txcds.data()));
}

bool Mesh::AddRenderGroup(std::shared_ptr<Material> material,
                          GLenum drawMode,
                          const std::vector<float>& verts,
                          const std::vector<float>& norms,
                          const std::vector<float>& txcds)
{
    return AddRenderGroup(material, drawMode, (unsigned int)verts.size() / 3,
                         verts.data(),
                         (norms.empty() ? nullptr : norms.data()),
                         (txcds.empty() ? nullptr : txcds.data()));
}

bool Mesh::AddRenderGroup(std::shared_ptr<Material> material,
                          GLenum drawMode,
                          unsigned int vertCount,
                          const float * verts,
                          const float * norms,
                          const float * txcds)
{
    RenderGroup group;
    group.vertCount = (GLsizei)vertCount;
    group.material = material;
    group.drawMode = drawMode;

    glGenVertexArrays(1, &group.glVAO);
    glBindVertexArray(group.glVAO);

    glGenBuffers(3, group.glVBOs);

    glBindBuffer(GL_ARRAY_BUFFER, group.glVBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertCount, verts, GL_STATIC_DRAW);
    glVertexAttribPointer(Mesh::AttrID::VERTS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(Mesh::AttrID::VERTS);

    if (norms)
    {
        glBindBuffer(GL_ARRAY_BUFFER, group.glVBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertCount, norms, GL_STATIC_DRAW);
        glVertexAttribPointer(Mesh::AttrID::NORMS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(Mesh::AttrID::NORMS);
    }

    if (txcds)
    {
        glBindBuffer(GL_ARRAY_BUFFER, group.glVBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * vertCount, txcds, GL_STATIC_DRAW);
        glVertexAttribPointer(Mesh::AttrID::TXCDS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(Mesh::AttrID::TXCDS);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    _renderGroups.push_back(group);
    return true;
}

std::shared_ptr<FileMesh> FileMesh::Create(const std::string& filename)
{
    App * app = App::GetInst();
    AssetId id = app->GetMeshIndex()->GetId(filename);
    std::shared_ptr<Mesh> ptr = app->GetMeshCache()->Get(id);
    if (!ptr)
    {
        ptr.reset(new FileMesh(filename));
        app->GetMeshCache()->Add(id, ptr);
    }
    return std::dynamic_pointer_cast<FileMesh>(ptr);
}

FileMesh::FileMesh(const std::string& filename)
    : Mesh()
    , _filename(filename)
{
    DuskLogInfo("Loading model from '%s'", _filename.c_str());

    std::string ext = GetExtension(_filename);
    if (ext == "obj")
    {
        LoadOBJ(_filename);
    }
    else if (ext == "dmf" || ext == "dmfz")
    {
        //LoadDMF(_filename);
    }
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

        std::shared_ptr<Material> material;
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

            material.reset(new Material(
                glm::vec4(mat->ambient[0], mat->ambient[1], mat->ambient[2], 1.0f),
                glm::vec4(mat->diffuse[0], mat->diffuse[1], mat->diffuse[2], 1.0f),
                glm::vec4(mat->specular[0], mat->specular[1], mat->specular[2], 1.0f),
                mat->shininess, mat->dissolve,
                ambient_texname,
                diffuse_texname,
                specular_texname,
                bump_texname
            ));
        }
        AddRenderGroup(material, GL_TRIANGLES, verts, norms, txcds);
    }

    return true;
}

std::shared_ptr<PlaneMesh> PlaneMesh::Create(std::shared_ptr<Material> material,
                                             unsigned int rows, unsigned int cols,
                                             float width, float height)
{
    std::stringstream ss;
    ss << "PlaneMesh[" << rows << ","
                       << cols << ","
                       << width << ","
                       << height << "]";

    App * app = App::GetInst();
    AssetId id = app->GetMeshIndex()->GetId(ss.str());
    std::shared_ptr<Mesh> ptr = app->GetMeshCache()->Get(id);
    if (!ptr)
    {
        ptr.reset(new PlaneMesh(material, rows, cols, width, height));
        app->GetMeshCache()->Add(id, ptr);
    }
    return std::dynamic_pointer_cast<PlaneMesh>(ptr);
}

PlaneMesh::PlaneMesh(std::shared_ptr<Material> material,
                     unsigned int rows, unsigned int cols,
                     float width, float height)
    : Mesh()
    , _material(material)
    , _rows(rows)
    , _cols(cols)
    , _width(width)
    , _height(height)
{
    float squareWidth = _width / (float)_cols;
    float squareHeight = _height / (float)_rows;

    int vertCount = (_rows * _cols) + (_rows - 1) * (_cols - 2);

    std::vector<glm::vec3> tmpVerts;
    std::vector<glm::vec3> tmpNorms;
    std::vector<glm::vec2> tmpTxcds;
    tmpVerts.resize(_rows * _cols);
    tmpNorms.resize(_rows * _cols);
    tmpTxcds.resize(_rows * _cols);

    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> norms;
    std::vector<glm::vec2> txcds;
    verts.resize(vertCount);
    norms.resize(vertCount);
    txcds.resize(vertCount);

    // Generate all possible points in a grid
    int i = 0;
    for (unsigned int row = 0; row < _rows; row++)
    {
        for (unsigned int col = 0; col < _cols; col++)
        {
            tmpVerts[i] = glm::vec3((float)col * squareWidth, 0.0f, (float)row * squareHeight);
            tmpNorms[i] = glm::vec3(0.0f, 1.0f, 0.0f);
            tmpTxcds[i] = glm::vec2((float)col / (float)_cols, (float)row / (float)_rows);
            ++i;
        }
    }

    // Build the triangle strip from the generated points
    i = 0;
    for (unsigned int row = 0; row < _rows - 1; row++)
    {
        if ((row & 1) == 0)
        {
            // even rows
            for (unsigned int col = 0; col < _cols; col++)
            {
                verts[i] = tmpVerts[col + row * _cols];
                norms[i] = tmpNorms[col + row * _cols];
                txcds[i] = tmpTxcds[col + row * _cols];
                ++i;
                verts[i] = tmpVerts[col + (row + 1) * _cols];
                norms[i] = tmpNorms[col + (row + 1) * _cols];
                txcds[i] = tmpTxcds[col + (row + 1) * _cols];
                ++i;
            }
        }
        else
        {
            // odd rows
            for (unsigned int col = _cols - 1; col > 0; col--)
            {
                verts[i] = tmpVerts[col + (row + 1) * _cols];
                norms[i] = tmpNorms[col + (row + 1) * _cols];
                txcds[i] = tmpTxcds[col + (row + 1) * _cols];
                ++i;
                verts[i] = tmpVerts[col - 1 + row * _cols];
                norms[i] = tmpNorms[col - 1 + row * _cols];
                txcds[i] = tmpTxcds[col - 1 + row * _cols];
                ++i;
            }
        }
    }

    // If ending on an odd row, finish off the last point
    if ((_rows & 1) && _rows > 2)
    {
        verts[i] = tmpVerts[(_rows - 1) * _cols];
        norms[i] = tmpNorms[(_rows - 1) * _cols];
        txcds[i] = tmpTxcds[(_rows - 1) * _cols];
        ++i;
    }

    AddRenderGroup(_material, GL_TRIANGLE_STRIP, verts, norms, txcds);
}

std::shared_ptr<CuboidMesh>
CuboidMesh::Create(std::shared_ptr<Material> material,
                   float width,
                   float height,
                   float depth)
{
    std::stringstream ss;
    ss << "CuboidMesh[" << width << ","
                       << height << ","
                       << depth << "]";

    App * app = App::GetInst();
    AssetId id = app->GetMeshIndex()->GetId(ss.str());
    std::shared_ptr<Mesh> ptr = app->GetMeshCache()->Get(id);
    if (!ptr)
    {
        ptr.reset(new CuboidMesh(material, width, height, depth));
        app->GetMeshCache()->Add(id, ptr);
    }
    return std::dynamic_pointer_cast<CuboidMesh>(ptr);
}

CuboidMesh::CuboidMesh(std::shared_ptr<Material> material,
                       float width,
                       float height,
                       float depth)
    : Mesh()
    , _material(material)
    , _width(width)
    , _height(height)
    , _depth(depth)
{
    std::vector<glm::vec3> verts = {
        glm::vec3(0,      0,       _depth),
        glm::vec3(0,      _height, _depth),
        glm::vec3(0,      _height, 0),

        glm::vec3(0,      0,       _depth),
        glm::vec3(0,      _height, 0),
        glm::vec3(0,      0,       0),

        glm::vec3(_width, 0,       0),
        glm::vec3(_width, _height, 0),
        glm::vec3(_width, _height, _depth),

        glm::vec3(_width, 0,       0),
        glm::vec3(_width, _height, _depth),
        glm::vec3(_width, 0,       _depth),

        glm::vec3(_width, 0,       0),
        glm::vec3(_width, 0,       _depth),
        glm::vec3(0,      0,       _depth),

        glm::vec3(_width, 0,       0),
        glm::vec3(0,      0,       _depth),
        glm::vec3(0,      0,       0),

        glm::vec3(_width, _height, _depth),
        glm::vec3(_width, _height, 0),
        glm::vec3(0,      _height, 0),

        glm::vec3(_width, _height, _depth),
        glm::vec3(0,      _height, 0),
        glm::vec3(0,      _height, _depth),

        glm::vec3(0,      0,       0),
        glm::vec3(_width, _height, 0),
        glm::vec3(_width, 0,       0),

        glm::vec3(0,      0,       0),
        glm::vec3(_width, _height, 0),
        glm::vec3(_width, 0,       0),

        glm::vec3(_width, 0,       _depth),
        glm::vec3(_width, _height, _depth),
        glm::vec3(0,      _height, _depth),

        glm::vec3(_width, 0,       _depth),
        glm::vec3(0,      _height, _depth),
        glm::vec3(0,      0,       _depth),
    };

    // Center cube
    glm::vec3 offset = { _width * 0.5f, _height * 0.5f, _depth * 0.5f };
    for (auto& v : verts)
    {
        v -= offset;
    }

    std::vector<glm::vec3> norms = {
        glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0),
        glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0),
        glm::vec3(1, 0, 0),  glm::vec3(1, 0, 0),  glm::vec3(1, 0, 0),
        glm::vec3(1, 0, 0),  glm::vec3(1, 0, 0),  glm::vec3(1, 0, 0),
        glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0),
        glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0),
        glm::vec3(0, 1, 0),  glm::vec3(0, 1, 0),  glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),  glm::vec3(0, 1, 0),  glm::vec3(0, 1, 0),
        glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1),
        glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1),
        glm::vec3(0, 0, 1),  glm::vec3(0, 0, 1),  glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),  glm::vec3(0, 0, 1),  glm::vec3(0, 0, 1),
    };

    std::vector<glm::vec2> txcds = {
        glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1),
        glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 1),
        glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1),
        glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 1),
        glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1),
        glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 1),
        glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1),
        glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 1),
        glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1),
        glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 1),
        glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1),
        glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 1),
    };

    AddRenderGroup(_material, GL_TRIANGLES, verts, norms, txcds);
}

std::shared_ptr<CubeMesh>
CubeMesh::Create(std::shared_ptr<Material> material, float size)
{
    std::stringstream ss;
    ss << "CubeMesh[" << size << "]";

    App * app = App::GetInst();
    AssetId id = app->GetMeshIndex()->GetId(ss.str());
    std::shared_ptr<Mesh> ptr = app->GetMeshCache()->Get(id);
    if (!ptr)
    {
        ptr.reset(new CubeMesh(material, size));
        app->GetMeshCache()->Add(id, ptr);
    }
    return std::dynamic_pointer_cast<CubeMesh>(ptr);
}

std::shared_ptr<UVSphereMesh>
UVSphereMesh::Create(std::shared_ptr<Material> material,
                     unsigned int rows,
                     unsigned int cols,
                     float radius)
{
    std::stringstream ss;
    ss << "UVSphereMesh[" << rows << ","
                          << cols << ","
                          << radius << "]";

    App * app = App::GetInst();
    AssetId id = app->GetMeshIndex()->GetId(ss.str());
    std::shared_ptr<Mesh> ptr = app->GetMeshCache()->Get(id);
    if (!ptr)
    {
        ptr.reset(new UVSphereMesh(material, rows, cols, radius));
        app->GetMeshCache()->Add(id, ptr);
    }
    return std::dynamic_pointer_cast<UVSphereMesh>(ptr);
}

std::shared_ptr<IcoSphereMesh>
IcoSphereMesh::Create(std::shared_ptr<Material> material,
                     unsigned int subdivisions,
                     float radius)
{
    std::stringstream ss;
    ss << "IcoSphereMesh[" << subdivisions << ","
                           << radius << "]";

    App * app = App::GetInst();
    AssetId id = app->GetMeshIndex()->GetId(ss.str());
    std::shared_ptr<Mesh> ptr = app->GetMeshCache()->Get(id);
    if (!ptr)
    {
        ptr.reset(new IcoSphereMesh(material, subdivisions, radius));
        app->GetMeshCache()->Add(id, ptr);
    }
    return std::dynamic_pointer_cast<IcoSphereMesh>(ptr);
}

//bool UVSphereMesh::Load()
//{
    //mesh.vertices.emplace_back(0.0f, 1.0f, 0.0f);
	//for (uint32_t j = 0; j < parallels - 1; ++j)
	//{
	//	double const polar = M_PI * double(j+1) / double(parallels);
	//	double const sp = std::sin(polar);
	//	double const cp = std::cos(polar);
	//	for (uint32_t i = 0; i < meridians; ++i)
	//	{
	//		double const azimuth = 2.0 * M_PI * double(i) / double(meridians);
	//		double const sa = std::sin(azimuth);
	//		double const ca = std::cos(azimuth);
	//		double const x = sp * ca;
	//		double const y = cp;
	//		double const z = sp * sa;
	//		mesh.vertices.emplace_back(x, y, z);
	//	}
	//}
    //mesh.vertices.emplace_back(0.0f, -1.0f, 0.0f);

//    return Mesh::Load();
//}

std::shared_ptr<ConeMesh>
ConeMesh::Create(std::shared_ptr<Material> material,
       unsigned int points,
       float radius,
       float height)
{
    std::stringstream ss;
    ss << "ConeMesh[" << points << ","
                      << radius << ","
                      << height << "]";

    App * app = App::GetInst();
    AssetId id = app->GetMeshIndex()->GetId(ss.str());
    std::shared_ptr<Mesh> ptr = app->GetMeshCache()->Get(id);
    if (!ptr)
    {
       ptr.reset(new ConeMesh(material, points, radius, height));
       app->GetMeshCache()->Add(id, ptr);
    }
    return std::dynamic_pointer_cast<ConeMesh>(ptr);
}

ConeMesh::ConeMesh(std::shared_ptr<Material> material,
                   unsigned int points,
                   float radius,
                   float height)
    : Mesh()
    , _material(material)
    , _points(points)
    , _radius(radius)
    , _height(height)
{
    glm::vec3 tip = glm::vec3(0, _height * 0.5f, 0);
    glm::vec3 base = glm::vec3(0, -_height * 0.5f, 0);

    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> norms;
    std::vector<glm::vec2> txcds;

    float angleSlice = (glm::pi<float>() * 2.0f) / (float)_points;

    std::vector<glm::vec3> basePoints;
    for (unsigned int i = 0; i < _points; ++i)
    {
        float x = cosf(angleSlice * (float)i) * _radius;
        float z = sinf(angleSlice * (float)i) * _radius;
        basePoints.push_back(glm::vec3(x, -_height * 0.5f, z));
    }

    for (unsigned int i = 0; i < _points; ++i)
    {
        verts.push_back(basePoints[i]);
        verts.push_back(tip);
        if (i != _points - 1)
        {
            verts.push_back(basePoints[i + 1]);
        }
        else
        {
            verts.push_back(basePoints[0]);
        }

        verts.push_back(basePoints[i]);
        verts.push_back(base);
        if (i != _points - 1)
        {
            verts.push_back(basePoints[i + 1]);
        }
        else
        {
            verts.push_back(basePoints[0]);
        }
    }

    AddRenderGroup(_material, GL_TRIANGLES, verts, norms, txcds);
}

} // namespace dusk
