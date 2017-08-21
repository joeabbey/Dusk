#ifndef DUSK_UTIL_HPP
#define DUSK_UTIL_HPP

#include <dusk/Config.hpp>

#include <string>

namespace dusk {

struct Box
{
public:

    glm::vec3 Min;
    glm::vec3 Max;

    Box() = default;
    Box(glm::vec3 min, glm::vec3 max)
        : Min(min)
        , Max(max)
    { }

    glm::vec3 GetSize() { return Max - Min; }

    Box operator+(const Box& rhs)
    {
        Box box;
        box.Min = glm::min(Min, rhs.Min);
        box.Max = glm::max(Max, rhs.Max);
        return box;
    }

    Box& operator+=(const Box& rhs)
    {
        Min = glm::min(Min, rhs.Min);
        Max = glm::max(Max, rhs.Max);
        return *this;
    }

};

size_t GetGLTypeSize(GLenum type);

void CleanSlashes(std::string& path);

std::string GetDirname(std::string path);
std::string GetBasename(std::string path);
std::string GetExtension(std::string path);

} // namespace dusk

#endif // DUSK_UTIL_HPP
