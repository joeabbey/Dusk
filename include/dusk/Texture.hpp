#ifndef DUSK_TEXTURE_HPP
#define DUSK_TEXTURE_HPP

#include <dusk/Config.hpp>

#include <dusk/Event.hpp>
#include <string>
#include <vector>

namespace dusk {

class Texture
{
public:

    DISALLOW_COPY_AND_ASSIGN(Texture);

    Texture() = default;
    Texture(const std::string& filename)
    {
        LoadFromFile(filename);
    }

    Texture(const glm::uvec2& size, const std::vector<uint8_t>& data, GLenum type = GL_RGBA)
    {
        LoadFromMemory(size, data, type);
    }

    Texture(Texture&& rhs)
    {
        _glID = rhs._glID;
        rhs._glID = 0;
    }

    virtual ~Texture()
    {
        if (_glID > 0) glDeleteTextures(1, &_glID);
    }

    bool LoadFromFile(const std::string& filename);

    bool LoadFromMemory(const glm::uvec2& size, const std::vector<uint8_t>& data, GLenum type = GL_RGBA);

    void Bind();

    bool IsLoaded() const { return _loaded; }

private:

    bool LoadGL(const glm::uvec2& size, const std::vector<uint8_t>& data, GLenum type);

    bool _loaded = false;

    GLuint _glID = 0;

}; // class Texture

} // namespace dusk

#endif // DUSK_TEXTURE_HPP
