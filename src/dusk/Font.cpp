#include "dusk/Font.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>
#include <dusk/App.hpp>

namespace dusk
{

Font::Font(const std::string& filename, unsigned int size)
    : _filename(filename)
    , _size(size)
{
    long int fileSize;

    DuskLogInfo("Loading font '%s'", filename.c_str());

    FILE * fp = fopen(filename.c_str(), "rb");
    if (!fp)
    {
        DuskLogError("Failed to open font '%s'", filename.c_str());
        return;
    }

    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);

    _buffer.resize(fileSize);

    fread(&_buffer[0], fileSize, 1, fp);
    fclose(fp);

    stbtt_InitFont(&_stbFontInfo, _buffer.data(), 0);
}

Text::Text(const std::string& text, std::shared_ptr<Font> font, Shader * shader /*= nullptr*/)
    : _shader(shader)
    , _baseTransform(1)
    , _position(0)
    , _rotation(0)
    , _scale(1)
    , _color(1)
    , _text(text)
    , _font(font)
    , _glTexture(0)
    , _glVAO(0)
    , _glVBOs()
{
    _glVBOs[0] = _glVBOs[1] = 0;

    App * app = App::GetInst();

    if (!_shader)
    {
        _shader = app->GetDefaultTextShader();
    }

    if (!font)
    {
        _font = app->GetDefaultFont();
    }

    const float vertices[] =
    {
        -1, -1, 0,
        -1,  1, 0,
         1,  1, 0,
        -1, -1, 0,
         1,  1, 0,
         1, -1, 0,
    };

    const float uvs[] =
    {
        0, 1,
        0, 0,
        1, 0,
        0, 1,
        1, 0,
        1, 1,
    };

    glGenVertexArrays(1, &_glVAO);
    glBindVertexArray(_glVAO);

    glGenBuffers(2, _glVBOs);

    glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 18, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, _glVBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 12, uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    _shader->Bind();

    glUniform1i(glGetUniformLocation(_shader->GetGLProgram(), "u_Texture"), TEXTURE_ID);
}

Text::~Text()
{
    glDeleteBuffers(2, _glVBOs);
    glDeleteVertexArrays(1, &_glVAO);
    glDeleteTextures(1, &_glTexture);
}

void Text::SetText(const std::string& text)
{
    _text = text;
    _invalid = true;
}

void Text::SetBaseTransform(const glm::mat4& baseTransform)
{
    _baseTransform = baseTransform;
}

void Text::SetPosition(const glm::vec3& pos)
{
    _position = pos;
}

void Text::SetRotation(const glm::vec3& rot)
{
    _rotation = rot;
}

void Text::SetScale(const glm::vec3& scale)
{
    _scale = scale;
}

void Text::SetColor(const glm::vec4& color)
{
    _color = color;
}

void Text::Render()
{
    if (_invalid)
    {
        _invalid = false;

        stbtt_fontinfo * info = _font->GetSTBFontInfo();

        int ascent, descent, lineGap, x = 0;
        stbtt_GetFontVMetrics(info, &ascent, &descent, &lineGap);

        float scale = stbtt_ScaleForPixelHeight(info, _font->GetSize());
        ascent *= scale;

        std::vector<uint8_t> texture;
        texture.resize(TEXTURE_WIDTH * TEXTURE_HEIGHT);

        for (size_t i = 0; i < _text.size(); ++i)
        {
            int x1, y1, x2, y2;
            stbtt_GetCodepointBitmapBox(info, _text[i], scale, scale, &x1, &y1, &x2, &y2);

            int y = ascent + y1;

            int byteOffset = x + y  * TEXTURE_WIDTH;
            stbtt_MakeCodepointBitmap(info, texture.data() + byteOffset, x2 - x1, y2 - y1, TEXTURE_WIDTH, scale, scale, _text[i]);

            int ax;
            stbtt_GetCodepointHMetrics(info, _text[i], &ax, nullptr);
            x += ax * scale;

            if (i != _text.size() - 1)
            {
                auto kern = stbtt_GetCodepointKernAdvance(info, _text[i], _text[i + 1]);
                x += kern * scale;
            }
        }

        if (_glTexture > 0)
        {
            glDeleteTextures(1, &_glTexture);
        }

        glGenTextures(1, &_glTexture);
        glBindTexture(GL_TEXTURE_2D, _glTexture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, texture.data());
        //glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    _shader->Bind();

    glUniform4fv(glGetUniformLocation(_shader->GetGLProgram(), "u_Color"), 1, (float *)&_color);

    glActiveTexture(GL_TEXTURE0 + TEXTURE_ID);
    glBindTexture(GL_TEXTURE_2D, _glTexture);

    glBindVertexArray(_glVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

} // namespace dusk
