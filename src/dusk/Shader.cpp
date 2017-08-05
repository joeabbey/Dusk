#include "dusk/Shader.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>

#include <fstream>
#include <sstream>

namespace dusk {

std::queue<GLuint> ShaderProgram::_AvailableUniformBufferBindings;
std::unordered_map<std::string, ShaderProgram::UniformBufferRecord> ShaderProgram::_UniformBuffers;

bool Shader::LoadFromFile(const std::string& filename)
{
    GLuint type = GL_INVALID_ENUM;
    std::ifstream file(filename);
    std::string code((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    file.close();

    if (filename.find(".vs.glsl") != std::string::npos)
    {
        type = GL_VERTEX_SHADER;
    }
    else if (filename.find(".fs.glsl") != std::string::npos)
    {
        type = GL_FRAGMENT_SHADER;
    }
    else if (filename.find(".gs.glsl") != std::string::npos)
    {
        type = GL_GEOMETRY_SHADER;
    }
    // Requires OpenGL 4.0+
    /*
    else if (filename.find(".tcs.glsl") != std::string::npos)
    {
        type = GL_TESS_CONTROL_SHADER;
    }
    else if (filename.find(".tes.glsl") != std::string::npos)
    {
        type = GL_TESS_EVALUATION_SHADER;
    }
    */
    // Requires OpenGL 4.3+
    /*
    else if (filename.find(".cs.glsl") != std::string::npos)
    {
        type = GL_COMPUTE_SHADER;
    }
    */
    else
    {
        DuskLogError("Unable to infer shader type for '%s'", filename.c_str());
        return false;
    }

    DuskLogLoad("Loading %s shader from '%s'", GetShaderTypeString(type).c_str(), filename.c_str());
    std::string processedCode = PreProcess(type, code, GetDirname(filename));
    return Compile(type, processedCode);
}

bool Shader::LoadFromString(GLuint type, const std::string& code)
{
    DuskLogLoad("Loading %s shader from string", GetShaderTypeString(type).c_str());
    std::string processedCode = PreProcess(type, code);
    return Compile(type, processedCode);
}

std::string Shader::GetShaderTypeString(GLuint type)
{
    switch (type)
    {
    case GL_VERTEX_SHADER:
        return "Vertex";

    case GL_FRAGMENT_SHADER:
        return "Fragment";

    case GL_GEOMETRY_SHADER:
        return "Geometry";

    // Requires OpenGL 4.0+
    /*
    case GL_TESS_CONTROL_SHADER:
        return "Tessellation Control";

    case GL_TESS_EVALUATION_SHADER:
        return "Tessellation Evaluation";
    */

    // Requires OpenGL 4.3+
    /*
    case GL_COMPUTE_SHADER:
        return "Compute";
    */

    default:
        break;
    }

    return "Unknown";
}

std::string Shader::PreProcess(GLuint type, const std::string& code, const std::string& basedir /* = "" */)
{
    std::istringstream iss(code);
    std::string processedCode;
    std::string line;

    while (std::getline(iss, line))
    {
        if (line[0] == '#')
        {
            if (0 == line.compare(0, strlen("#include"), "#include"))
            {
                std::string incFilename = (basedir.empty() ? "" : basedir + "/") + line.substr(strlen("#include") + 1);

                std::ifstream incFile(incFilename);
                std::string incCode((std::istreambuf_iterator<char>(incFile)),
                                     std::istreambuf_iterator<char>());
                incFile.close();

                processedCode += PreProcess(type, incCode, GetDirname(incFilename));

                continue;
            }
        }

        // TODO: Support more preprocessing

        processedCode += line;
        processedCode += "\n";
    }

    return processedCode;
}

bool Shader::Compile(GLuint type, const std::string& code)
{
    GLint compiled = GL_FALSE;
    const char * bufferPtr = code.c_str();

    _compiled = false;

    if (type == GL_INVALID_ENUM)
    {
        DuskLogError("Invalid shader type");
        return false;
    }

    if (_glId > 0)
    {
        glDeleteShader(_glId);
    }

    _glId = glCreateShader(type);

    if (0 == _glId)
    {
        DuskLogError("Failed to create shader");
        return false;
    }

    glShaderSource(_glId, 1, (const GLchar **)&bufferPtr, nullptr);
    glCompileShader(_glId);

    glGetShaderiv(_glId, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        DuskLogError("Failed to compile shader");
        PrintCode(code);
        PrintLog();
        return false;
    }

    _compiled = true;
    return true;
}

void Shader::PrintLog()
{
    std::vector<char> shaderLog;
    GLint logSize, retSize;

    if (!glIsShader(_glId))
    {
        DuskLogError("Cannot print shader log, %d is not a shader", _glId);
        return;
    }

    glGetShaderiv(_glId, GL_INFO_LOG_LENGTH, &logSize);

    shaderLog.resize(logSize);
    glGetShaderInfoLog(_glId, logSize, &retSize, shaderLog.data());

    DuskLogInfo("Log for shader %d:\n%s", _glId, shaderLog.data());
}

void Shader::PrintCode(const std::string& code)
{
    std::istringstream iss(code);

    unsigned int lineNum = 1;
    std::string line;
    while (std::getline(iss, line))
    {
        if (lineNum < 10) printf(" ");

        printf("%d: %s\n", lineNum++, line.c_str());
    }
}

ShaderProgram::ShaderProgram(const std::vector<std::string>& filenames)
{
    for (auto& filename : filenames)
    {
        Attach(Shader(filename));
    }
    Link();
}

ShaderProgram::~ShaderProgram()
{
    if (_glId > 0) glDeleteProgram(_glId);
}

void ShaderProgram::InitializeUniformBuffers()
{
    int maxBindings;
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings);
    DuskLogVerbose("Max UBO Bindings %d", maxBindings);

    for (int i = 0; i < maxBindings; ++i)
    {
        _AvailableUniformBufferBindings.push(static_cast<GLuint>(i));
    }

    int tmp;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &tmp);
    DuskLogVerbose("Max UBO Size %d", tmp);

    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &tmp);
    DuskLogVerbose("Max Vertex UBOs %d", tmp);

    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &tmp);
    DuskLogVerbose("Max Fragment UBOs %d", tmp);

    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &tmp);
    DuskLogVerbose("Max Geometry UBOs %d", tmp);
}

void ShaderProgram::Attach(Shader&& shader)
{
    if (0 == _glId)
    {
        _glId = glCreateProgram();
    }

    glAttachShader(_glId, shader.GetGLId());
    _shaders.push_back(std::move(shader));
}

bool ShaderProgram::Link()
{
    GLint linked = GL_FALSE;

    if (IsLinked()) return true;

    if (0 == _glId || _shaders.empty())
    {
        DuskLogError("Cannot link an empty shader program");
        return false;
    }

    glLinkProgram(_glId);

    glGetProgramiv(_glId, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        DuskLogError("Failed to link shader program");
        PrintLog();
        return false;
    }

    for (auto& shader : _shaders)
    {
        glDetachShader(_glId, shader.GetGLId());
    }
    _shaders.clear();

    CacheUniforms();
    CacheAttributes();

    _linked = true;
    return true;
}

void ShaderProgram::Bind()
{
    glUseProgram(_glId);
}

void ShaderProgram::SetUniformBufferData(const std::string& name, GLvoid * data)
{
    if (_UniformBuffers.find(name) == _UniformBuffers.end())
    {
        DuskLogWarn("Attempt to set data to uknown UBO");
        return;
    }

    UniformBufferRecord& record = _UniformBuffers[name];

    glBindBuffer(GL_UNIFORM_BUFFER, record.GLID);

    GLvoid* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, data, record.Size);

    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

GLint ShaderProgram::GetAttributeLocation(const std::string& name) const
{
    if (_attributes.find(name) == _attributes.end()) return std::numeric_limits<GLint>::max();
    return _attributes.at(name);
}

GLint ShaderProgram::GetUniformLocation(const std::string& name) const
{
    if (_uniforms.find(name) == _uniforms.end()) return 0;
    return _uniforms.at(name).Location;
}

bool ShaderProgram::HasAttribute(const std::string& name) const
{
    return (_attributes.find(name) != _attributes.end());
}

bool ShaderProgram::HasUniform(const std::string& name) const
{
    return (_uniforms.find(name) != _uniforms.end());
}

void ShaderProgram::CacheUniforms()
{
    GLint tmpSize; // size of the variable
    GLenum tmpType; // type of the variable (float, vec3 or mat4, etc)

    GLchar buffer[256]; // variable name in GLSL
    GLsizei length; // name length

    GLint count;
    glGetProgramiv(_glId, GL_ACTIVE_UNIFORMS, &count);
    for (GLint i = 0; i < count; ++i)
    {
        glGetActiveUniform(_glId, (GLuint)i, sizeof(buffer), &length, &tmpSize, &tmpType, buffer);
        _uniforms.emplace(buffer, UniformRecord{
            glGetUniformLocation(_glId, buffer),
            tmpSize,
            tmpType
        });
    }

    const int STRIDE = 16;

    std::string name;
    UniformRecord uniform;
    std::unordered_map<std::string, size_t> uboSizes;
    for (auto pair : _uniforms)
    {
        std::tie(name, uniform) = pair;

        if (name.find('.') == std::string::npos) continue;

        std::string uboName = name.substr(0, name.find('.'));
        if (uboSizes.find(uboName) == uboSizes.end())
        {
            uboSizes.emplace(uboName, 0);
        }

        size_t uniformSize = uniform.Size * GetGLTypeSize(uniform.Type);
        size_t allowedSize = STRIDE - (uboSizes[uboName] % STRIDE);

        if (uniformSize > allowedSize && allowedSize < STRIDE)
        {
            uboSizes[uboName] += allowedSize; // Pad to stride
        }

        uboSizes[uboName] += uniformSize;
    }

    size_t size;
    for (auto pair : uboSizes)
    {
        std::tie(name, size) = pair;

        if (size % STRIDE > 0)
        {
            size += STRIDE - (size % STRIDE);
        }

        if (_UniformBuffers.find(name) == _UniformBuffers.end())
        {
            if (_AvailableUniformBufferBindings.empty())
            {
                DuskLogError("Reached UBO binding limit");
                continue;
            }

            std::vector<uint8_t> dummy(size, 0);

            GLuint uboId = 0;
            glGenBuffers(1, &uboId);
            if (0 == uboId)
            {
                DuskLogError("Failed to create UBO");
                continue;
            }

            glBindBuffer(GL_UNIFORM_BUFFER, uboId);
            glBufferData(GL_UNIFORM_BUFFER, size, dummy.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBufferBase(GL_UNIFORM_BUFFER, _AvailableUniformBufferBindings.front(), uboId);

            DuskLogVerbose("Binding UBO %s to %u", name.c_str(), _AvailableUniformBufferBindings.front());
            _UniformBuffers.emplace(name, UniformBufferRecord{
                uboId,
                _AvailableUniformBufferBindings.front(),
                size,
            });
            _AvailableUniformBufferBindings.pop();
        }

        GLuint blockIndex = glGetUniformBlockIndex(_glId, name.c_str());
        glUniformBlockBinding(_glId, blockIndex, _UniformBuffers[name].Binding);
    }
}

void ShaderProgram::CacheAttributes()
{
    GLint size;
    GLenum type;

    GLchar name[256];
    GLsizei length;

    GLint count;
    glGetProgramiv(_glId, GL_ACTIVE_ATTRIBUTES, &count);
    for (GLint i = 0; i < count; ++i)
    {
        glGetActiveAttrib(_glId, (GLuint)i, sizeof(name), &length, &size, &type, name);
        _attributes.emplace(name, glGetAttribLocation(_glId, name));
    }
}

void ShaderProgram::PrintLog()
{
    // TODO: Convert to std::array

    std::string programLog;
    GLint logSize, retSize;

    if (!glIsProgram(_glId))
    {
        DuskLogError("Cannot print shader program log, %d is not a shader program", _glId);
        return;
    }

    glGetProgramiv(_glId, GL_INFO_LOG_LENGTH, &logSize);

    programLog.resize(logSize);
    glGetProgramInfoLog(_glId, logSize, &retSize, &programLog[0]);

    DuskLogInfo("Log for shader program %d:\n%s", _glId, programLog.c_str());
}

} // namespace dusk
