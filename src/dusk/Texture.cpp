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

    // OpenGL is weird
    stbi_set_flip_vertically_on_load(true);

    int width, height, comp;
    unsigned char * image = stbi_load(_filename.c_str(), &width, &height, &comp, STBI_rgb_alpha);

    if (!image)
    {
        DuskLogError("Loading image failed '%s'", _filename.c_str());
        goto error;
    }

    glGenTextures(1, &_glID);

    if (0 == _glID)
    {
        DuskLogError("Failed to create GL Texture");
        goto error;
    }

    glBindTexture(GL_TEXTURE_2D, _glID);
    DuskLogInfo("Binding image '%s' to ID %u", _filename.c_str(), _glID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);
    image = nullptr;

    glBindTexture(GL_TEXTURE_2D, 0);

    DuskBenchEnd("Texture::Load()");
    return true;

error:

    stbi_image_free(image);
    image = nullptr;

    glDeleteTextures(1, &_glID);
    _glID = 0;

    glBindTexture(GL_TEXTURE_2D, 0);

    return false;
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
