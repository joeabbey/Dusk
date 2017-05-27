#include "dusk/Texture.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>

namespace dusk {

Texture::Texture(const std::string& filename)
    : _filename(filename)
    , _glID(0)
{ }

Texture::~Texture()
{
    Free();
}

bool Texture::Load()
{
    DuskBenchStart();

    DuskLogInfo("Loading image '%s'", _filename.c_str());

    int width, height, comp;
    unsigned char * image = stbi_load(_filename.c_str(), &width, &height, &comp, STBI_rgb_alpha);

    if (NULL == image)
    {
        DuskLogError("Loading image failed '%s'", _filename.c_str());
        return false;
    }

    glGenTextures(1, &_glID);
    glBindTexture(GL_TEXTURE_2D, _glID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    DuskBenchEnd("Texture::Load()");
    return true;
}

void Texture::Free()
{
    glDeleteTextures(1, &_glID);
    _glID = 0;
}

void Texture::Bind()
{
    glBindTexture(GL_TEXTURE_2D, _glID);
}

} // namespace dusk
