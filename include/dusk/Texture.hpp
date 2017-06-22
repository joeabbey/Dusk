#ifndef DUSK_TEXTURE_HPP
#define DUSK_TEXTURE_HPP

#include <dusk/Config.hpp>

#include <dusk/EventDispatcher.hpp>
#include <string>
#include <memory>

namespace dusk {

class Texture
    : public std::enable_shared_from_this<Texture>
    , public IEventDispatcher
{
public:

    DISALLOW_COPY_AND_ASSIGN(Texture);

    static std::shared_ptr<Texture> Create(const std::string& filename);
    ~Texture();

    bool Load();
    void Free();

    void Bind();

private:

    Texture(const std::string& filename);

    std::string _filename;

    GLuint _glID;

}; // class Texture

} // namespace dusk

#endif // DUSK_TEXTURE_HPP
