#ifndef DUSK_SHADER_HPP
#define DUSK_SHADER_HPP

#include <dusk/Config.hpp>

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>

namespace dusk {

class ShaderProgram;

class Shader
{
    friend class ShaderProgram;

public:

    DISALLOW_COPY_AND_ASSIGN(Shader);

    Shader() = default;

    Shader(const std::string& filename)
    {
        LoadFromFile(filename);
    }

    Shader(GLuint type, const std::string& code)
    {
        LoadFromString(type, code);
    }

    Shader(Shader&& rhs)
    {
        _glId = rhs._glId;
        rhs._glId = 0;
    }

    virtual ~Shader()
    {
        if (_glId > 0) glDeleteShader(_glId);
    }

    bool LoadFromFile(const std::string& filename);
    bool LoadFromString(GLuint type, const std::string& code);

    bool IsCompiled() const { return _compiled; }

private:

    std::string GetShaderTypeString(GLuint type);

    std::string PreProcess(GLuint type, const std::string& code, const std::string& basedir = "");

    bool Compile(GLuint type, const std::string& code);

    void PrintLog();
    void PrintCode(const std::string& code);

    GLuint GetGLId() const { return _glId; };

    bool _compiled = false;

    GLuint _glId = 0;

};

class ShaderProgram
{
public:

    /// Class Boilerplate

    DISALLOW_COPY_AND_ASSIGN(ShaderProgram);

    ShaderProgram() = default;
    ShaderProgram(const std::vector<std::string>& filenames);
    virtual ~ShaderProgram();

    void Serialize(nlohmann::json& data);
    void Deserialize(nlohmann::json& data);

    /// Static Methods

    static void InitializeUniformBuffers();

    static void SetUniformBufferData(const std::string& name, GLvoid * data);

    // Methods

    void Attach(Shader&& shader);

    bool Link();

    void Bind();

    bool IsLinked() const { return _linked; }

    GLint GetAttributeLocation(const std::string& name) const;
    bool HasAttribute(const std::string& name) const;

    GLint GetUniformLocation(const std::string& name) const;
    bool HasUniform(const std::string& name) const;

    #include "Shader.inc.hpp"

private:

    struct UniformRecord
    {
        GLint  Location;
        GLint  Size;
        GLenum Type;
    };

    struct UniformBufferRecord
    {
        GLuint GLID;
        GLuint Binding;
        size_t Size;
    };

    void CacheUniforms();
    void CacheAttributes();

    void PrintLog();

    bool _linked = false;
    GLuint _glId = 0;

    std::vector<Shader> _shaders;
    std::unordered_map<std::string, UniformRecord> _uniforms;
    std::unordered_map<std::string, GLuint> _attributes;

    static std::queue<GLuint> _AvailableUniformBufferBindings;
    static std::unordered_map<std::string, UniformBufferRecord> _UniformBuffers;

};

} // namespace dusk

#endif // DUSK_SHADER_HPP
