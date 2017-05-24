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

    int mode;
    SDL_Surface * surf = NULL;

    DuskLogInfo("Loading image '%s'", _filename.c_str());

    surf = IMG_Load(_filename.c_str());
    if (NULL == surf)
    {
        DuskLogError("Loading image failed '%s', %s", _filename.c_str(), IMG_GetError());
        SDL_FreeSurface(surf);
        return false;
    }

    mode = (4 == surf->format->BytesPerPixel ? GL_RGBA : GL_RGB);

    glGenTextures(1, &_glID);
    glBindTexture(GL_TEXTURE_2D, _glID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, mode, surf->w, surf->h, 0, mode, GL_UNSIGNED_BYTE, surf->pixels);

    SDL_FreeSurface(surf);
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
