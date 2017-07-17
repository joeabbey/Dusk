#ifndef DUSK_FONT_HPP
#define DUSK_FONT_HPP

#include <dusk/Config.hpp>

#include <dusk/Shader.hpp>
#include <vector>

namespace dusk
{

class Font
{
public:

    Font(const std::string& filename, unsigned int size);
    virtual ~Font() = default;

    std::string GetFilename() const { return _filename; }

    unsigned int GetSize() const { return _size; }

    stbtt_fontinfo * GetSTBFontInfo() { return &_stbFontInfo; }
    const stbtt_fontinfo * GetSTBFontInfo() const { return &_stbFontInfo; }

private:

    std::string _filename;

    unsigned int _size;

    std::vector<uint8_t> _buffer;

    stbtt_fontinfo _stbFontInfo;

}; // class Font

class Text
{
public:

    Text(const std::string& text, std::shared_ptr<Font> font, Shader * shader = nullptr);
    virtual ~Text();

    void SetText(const std::string& text);

    void SetBaseTransform(const glm::mat4& baseTransform);

    void SetPosition(const glm::vec3& pos);
    inline glm::vec3 GetPosition() const { return _position; }

    void SetRotation(const glm::vec3& rot);
    inline glm::vec3 GetRotation() const { return _rotation; }

    void SetScale(const glm::vec3& scale);
    inline glm::vec3 GetScale() const { return _scale; }

    void SetColor(const glm::vec4& color);
    inline glm::vec4 GetColor() const { return _color; }

    void Render();

private:

    const GLint TEXTURE_ID = 0;
    const GLint ATTR_ID    = 0;
    const int TEXTURE_WIDTH  = 1024;
    const int TEXTURE_HEIGHT = 1024;

    bool _invalid = true;

    Shader * _shader;

    glm::mat4 _baseTransform;

    glm::vec3 _position;
    glm::vec3 _rotation;
    glm::vec3 _scale;

    glm::vec4 _color;

    std::string _text;

    std::shared_ptr<Font> _font;

    GLuint _glTexture;
    GLuint _glVAO;
    GLuint _glVBOs[2];

}; // class Text

} // namespace dusk

#endif // DUSK_FONT_HPP
