#include "dusk/Util.hpp"

namespace dusk {

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
        ? "."
        : std::string(path.begin(), path.end() - pivot - 1));
}

std::string GetBasename(std::string path)
{
    CleanSlashes(path);
    size_t pivot = path.find_last_of('/');
    return (pivot == std::string::npos
        ? "."
        : std::string(path.begin() + pivot + 1, path.end()));
}

std::string GetExtension(std::string path)
{
    size_t pivot = path.find_last_of('.');
    return (pivot == std::string::npos
        ? std::string()
        : std::string(path.begin() + pivot, path.end()));
}

} // namespace dusk
