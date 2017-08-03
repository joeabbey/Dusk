#include "dusk/Util.hpp"

namespace dusk {

size_t GetGLTypeSize(GLenum type)
{
    switch (type)
    {
    case GL_FLOAT:              return sizeof(GLfloat);
    case GL_FLOAT_VEC2:         return sizeof(glm::vec2);
    case GL_FLOAT_VEC3:         return sizeof(glm::vec3);
    case GL_FLOAT_VEC4:         return sizeof(glm::vec4);
    case GL_DOUBLE:             return sizeof(GLdouble);
    //case GL_DOUBLE_VEC2:        return sizeof(glm::dvec2);
    //case GL_DOUBLE_VEC3:        return sizeof(glm::dvec3);
    //case GL_DOUBLE_VEC4:        return sizeof(glm::dvec4);
    case GL_INT:                return sizeof(GLint);
    case GL_INT_VEC2:           return sizeof(glm::ivec2);
    case GL_INT_VEC3:           return sizeof(glm::ivec3);
    case GL_INT_VEC4:           return sizeof(glm::ivec4);
    case GL_UNSIGNED_INT:       return sizeof(GLuint);
    case GL_UNSIGNED_INT_VEC2:  return sizeof(glm::uvec2);
    case GL_UNSIGNED_INT_VEC3:  return sizeof(glm::uvec3);
    case GL_UNSIGNED_INT_VEC4:  return sizeof(glm::uvec4);
    case GL_BOOL:               return sizeof(GLboolean);
    case GL_BOOL_VEC2:          return sizeof(glm::bvec2);
    case GL_BOOL_VEC3:          return sizeof(glm::bvec3);
    case GL_BOOL_VEC4:          return sizeof(glm::bvec4);
    case GL_FLOAT_MAT2:         return sizeof(glm::mat2);
    case GL_FLOAT_MAT3:         return sizeof(glm::mat3);
    case GL_FLOAT_MAT4:         return sizeof(glm::mat4);
    case GL_FLOAT_MAT2x3:       return sizeof(glm::mat2x3);
    case GL_FLOAT_MAT2x4:       return sizeof(glm::mat2x4);
    case GL_FLOAT_MAT3x2:       return sizeof(glm::mat3x2);
    case GL_FLOAT_MAT3x4:       return sizeof(glm::mat3x4);
    case GL_FLOAT_MAT4x2:       return sizeof(glm::mat4x2);
    case GL_FLOAT_MAT4x3:       return sizeof(glm::mat4x3);
    //case GL_DOUBLE_MAT2:        return sizeof(glm::dmat2);
    //case GL_DOUBLE_MAT3:        return sizeof(glm::dmat3);
    //case GL_DOUBLE_MAT4:        return sizeof(glm::dmat4);
    //case GL_DOUBLE_MAT2x3:      return sizeof(glm::dmat2x3);
    //case GL_DOUBLE_MAT2x4:      return sizeof(glm::dmat2x4);
    //case GL_DOUBLE_MAT3x2:      return sizeof(glm::dmat3x2);
    //case GL_DOUBLE_MAT3x4:      return sizeof(glm::dmat3x4);
    //case GL_DOUBLE_MAT4x2:      return sizeof(glm::dmat4x2);
    //case GL_DOUBLE_MAT4x3:      return sizeof(glm::dmat4x3);

    // TODO: Finish
    }

    return 0;
}

void CleanSlashes(std::string& path)
{
    for (unsigned int i = 0; i < path.size(); ++i)
    {
        if (path[i] == '\\')
        {
            path[i] = '/';
        }
    }
}

std::string GetDirname(std::string path)
{
    CleanSlashes(path);
    size_t pivot = path.find_last_of('/');
    return (pivot == std::string::npos
        ? "./"
        : path.substr(0, pivot));
}

std::string GetBasename(std::string path)
{
    CleanSlashes(path);
    size_t pivot = path.find_last_of('/');
    return (pivot == std::string::npos
        ? std::string()
        : path.substr(pivot + 1));
}

std::string GetExtension(std::string path)
{
    size_t pivot = path.find_last_of('.');
    return (pivot == std::string::npos
        ? std::string()
        : path.substr(pivot + 1));
}

} // namespace dusk
