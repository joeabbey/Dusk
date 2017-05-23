#include "dusk/Dusk.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

namespace dusk {

bool LoadModelFromOBJ(const std::string& filename, Model * outModel)
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
    }

    return true;
}

bool LoadModelFromFile(const std::string& filename, Model * outModel)
{
    DuskBenchStart();
    DuskLogInfo("Loading model '%s'", filename.c_str());

    bool ret = false;

    std::string ext = GetExtension(filename);
    if (ext == "obj")
    {
        ret = LoadModelFromOBJ(filename, outModel);
    }

    DuskBenchEnd("LoadModelFromFile");
    return ret;
}

} // namespace dusk
