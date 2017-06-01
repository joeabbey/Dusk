#include "dusk/Shader.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>

#include <fstream>

namespace dusk {

std::unordered_map<std::string, Shader::ShaderData> Shader::_DataRecords;
int Shader::_MaxDataIndex = 0;

Shader::Shader(const std::string& name, const std::vector<std::string>& data, const std::vector<FileInfo>& files)
    : _name(name)
    , _files(files)
    , _glProgram(0)
{
    for (const auto& dataName : data)
    {
        _boundData.emplace(dataName, false);
    }
}

Shader::~Shader()
{
    Free();
}

bool Shader::Load()
{
    DuskBenchStart();

    // TODO: Cleanup the cleanup code

    std::vector<GLuint> shaderIds;
    GLint programLinked = GL_FALSE;

    if (_files.empty())
    {
        DuskLogWarn("Shader has no files '%s'", _name.c_str());
        return false;
    }

    _glProgram = glCreateProgram();

    if (0 == _glProgram)
    {
        DuskLogError("Failed to create shader program");
        goto error;
    }

    for (FileInfo& info : _files)
    {
        shaderIds.push_back(LoadShader(info.filename, info.type));
        if (0 == shaderIds.back())
        {
            DuskLogError("Failed to load shader program '%s'", info.filename.c_str());
            goto error;
        }
        glAttachShader(_glProgram, shaderIds.back());
    }

    glLinkProgram(_glProgram);

    glGetProgramiv(_glProgram, GL_LINK_STATUS, &programLinked);
    if (!programLinked)
    {
        DuskLogError("Failed to link shader program '%s'", _name.c_str());
        PrintShaderProgramLog(_glProgram);
        goto error;
    }

    for (GLuint id : shaderIds)
    {
        glDetachShader(_glProgram, id);
        glDeleteShader(id);
    }

    _loaded = true;

    for (const auto& it : _boundData)
    {
        auto tmp = _DataRecords.find(it.first);
        if (tmp == _DataRecords.end()) continue;

        UpdateData(it.first, nullptr, tmp->second.size);
        BindData(it.first);
    }

    DuskBenchEnd("Shader::Load()");
    return true;

error:

    for (GLuint id : shaderIds)
    {
        glDetachShader(_glProgram, id);
        glDeleteShader(id);
    }

    glDeleteShader(_glProgram);
    _glProgram = 0;

    return false;
}

void Shader::Free()
{
    glDeleteProgram(_glProgram);
    _glProgram = 0;
}

void Shader::Bind()
{
    glUseProgram(_glProgram);
}

GLint Shader::GetUniformLocation(const std::string& name)
{
    return glGetUniformLocation(_glProgram, name.c_str());
}

GLuint Shader::LoadShader(const std::string& filename, GLuint type)
{
    GLuint shader = 0;
    std::string buffer;
    GLint shaderCompiled = GL_FALSE;
    const char * bufferPtr;

    DuskLogInfo("Loading shader file '%s'", filename.c_str());

    shader = glCreateShader(type);
    if (0 == shader)
    {
        DuskLogError("Failed to create shader");
        goto error;
    }

    if (GL_INVALID_ENUM == shader)
    {
        DuskLogError("Invalid shader type %d", type);
        goto error;
    }

    if (!LoadFile(filename, buffer))
    {
        DuskLogError("Failed to open shader file '%s'", filename.c_str());
        goto error;
    }
    bufferPtr = buffer.c_str();

    glShaderSource(shader, 1, (const GLchar **)&bufferPtr, nullptr);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
    if (!shaderCompiled)
    {
        DuskLogError("Failed to compile shader '%s'", filename.c_str());
        PrintShaderLog(shader);
        goto error;
    }

    return shader;

error:

    glDeleteShader(shader);

    return 0;
}

bool Shader::LoadFile(const std::string& filename, std::string& buffer)
{
    bool retval = true;
    std::string dirname = GetDirname(filename);
    std::ifstream file(filename);
    std::string line;
    size_t size = 0;

    if (!file.is_open())
    {
        retval = false;
        goto error;
    }

    file.seekg(0, file.end);
    size = file.tellg();
    file.seekg(0, file.beg);

    buffer.reserve(buffer.size() + size);

    while (std::getline(file, line))
    {
        if (line[0] == '#')
        {
            if (0 == line.compare(0, strlen("#include"), "#include"))
            {
                std::string incFilename = dirname + "/" + line.substr(strlen("#include") + 1);
                if (filename == incFilename)
                {
                    DuskLogError("A shader cannot include itself");
                    goto error;
                }

                DuskLogInfo("Loading shader include '%s'", incFilename.c_str());
                if (!LoadFile(incFilename, buffer))
                {
                    DuskLogError("Failed to load include '%s'", incFilename.c_str());
                    goto error;
                }

                continue;
            }
        }

        buffer += line;
        buffer += '\n';
    }

error:

    file.close();
    return retval;
}

void Shader::PrintShaderLog(GLuint shader)
{
    std::string shaderLog;
    GLint logSize, retSize;

    if (!glIsShader(shader))
    {
        DuskLogError("Cannot print shader log, %d is not a shader", shader);
        return;
    }

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

    shaderLog.resize(logSize);
    glGetShaderInfoLog(shader, logSize, &retSize, &shaderLog[0]);

    DuskLogInfo("Log for shader %d:\n%s", shader, shaderLog.c_str());
}

void Shader::PrintShaderProgramLog(GLuint program)
{
    std::string programLog;
    GLint logSize, retSize;

    if (!glIsProgram(program))
    {
        DuskLogError("Cannot print shader program log, %d is not a shader program", program);
        return;
    }

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

    programLog.resize(logSize);
    glGetProgramInfoLog(program, logSize, &retSize, &programLog[0]);

    DuskLogInfo("Log for shader program %d:\n%s", program, programLog.c_str());
}

void Shader::BindData(const std::string& name)
{
    const auto& it = _DataRecords.find(name);

    if (_boundData[name]) return;

    if (it != _DataRecords.end())
    {
        if (_boundData.find(name) == _boundData.end())
        {
            _boundData.emplace(name, false);
        }

        if (_loaded)
        {
            glUseProgram(_glProgram);

            ShaderData& record = it->second;
            GLuint dataIndex = glGetUniformBlockIndex(_glProgram, name.c_str());
            glUniformBlockBinding(_glProgram, dataIndex, record.index);
            glBindBufferBase(GL_UNIFORM_BUFFER, record.index, record.glUBO);

            glUseProgram(0);

            _boundData[name] = true;

            DuskLogInfo("Binding shader %u to data %s", _glProgram, name.c_str());
        }
    }
    else
    {
        DuskLogError("Failed to bind shader to data %s, does not exist", name.c_str());
    }
}

void Shader::DefineData(const std::string& name, size_t size)
{
    const auto& it = _DataRecords.find(name);

    if (it == _DataRecords.end())
    {
        _DataRecords.emplace(name, ShaderData());
        ShaderData& record = _DataRecords[name];
        record.size = size;
        record.index = _MaxDataIndex;
        record.loaded = false;

        ++_MaxDataIndex;
    }
}

void Shader::UpdateData(const std::string& name, void * data, size_t size)
{
    const auto& it = _DataRecords.find(name);

    if (it == _DataRecords.end())
    {
        _DataRecords.emplace(name, ShaderData());
        ShaderData& record = _DataRecords[name];
        record.size = size;
        record.index = _MaxDataIndex;
        record.loaded = true;

        ++_MaxDataIndex;

        glGenBuffers(1, &record.glUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, record.glUBO);
        glBufferData(GL_UNIFORM_BUFFER, record.size, data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        DuskLogInfo("Added Shader Data %s at index %d", name.c_str(), record.index);
    }
    else
    {
        ShaderData& record = it->second;

        if (record.loaded)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, record.glUBO);
            GLvoid * dataPtr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
            memcpy(dataPtr, data, record.size);
            glUnmapBuffer(GL_UNIFORM_BUFFER);
        }
        else
        {
            record.loaded = true;

            glGenBuffers(1, &record.glUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, record.glUBO);
            glBufferData(GL_UNIFORM_BUFFER, record.size, data, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            DuskLogInfo("Added Shader Data %s at index %d", name.c_str(), record.index);
        }
    }
}

} // namespace dusk
