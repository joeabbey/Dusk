#ifndef DUSK_TEXTURE_HPP
#define DUSK_TEXTURE_HPP

#include <dusk/Config.hpp>

#include <string>

namespace dusk {

class Texture
{
public:

    DISALLOW_COPY_AND_ASSIGN(Texture);

    Texture(const std::string& filename);
    ~Texture();

    bool Load();
    void Free();

    void Bind();

private:

    std::string _filename;

    GLuint _glID;

}; // class Texture

} // namespace dusk

#endif // DUSK_TEXTURE_HPP
