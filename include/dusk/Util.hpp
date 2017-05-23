#ifndef DUSK_UTIL_HPP
#define DUSK_UTIL_HPP

#include <dusk/Config.hpp>

#include <string>

namespace dusk {

void CleanSlashes(std::string& path);

std::string GetDirname(std::string path);
std::string GetBasename(std::string path);
std::string GetExtension(std::string path);

} // namespace dusk

#endif // DUSK_UTIL_HPP
