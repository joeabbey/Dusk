#ifndef DUSK_SHADER_HPP
#define DUSK_SHADER_HPP

#include <dusk/Config.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace dusk {

class Shader
{
public:

    DISALLOW_COPY_AND_ASSIGN(Shader);

    struct FileInfo {
        GLuint type;
        std::string filename;
    };

    Shader(const std::string& name, const std::vector<FileInfo>& files);
    virtual ~Shader();

    bool IsLoaded() const { return _loaded; }

    std::string GetName() const { return _name; }

    bool Load();
    void Free();

    void Bind();

    GLint GetUniformLocation(const std::string& name);

    void BindData(const std::string& name);
    static void SetData(const std::string& name, void * data, size_t size);

private:

    struct ShaderData
    {
        GLuint glUBO;
        size_t size;
        int index;
    };

    static std::unordered_map<std::string, ShaderData> _DataRecords;
    static int _MaxDataIndex;

    bool _loaded;
    std::string _name;
    std::vector<FileInfo> _files;

    std::unordered_map<std::string, bool> _boundData;
    GLuint _glProgram;

    GLuint LoadShader(const std::string& filename, GLuint type);

    static bool LoadFile(const std::string& filename, std::string& buffer);

    static void PrintShaderLog(GLuint shader);
    static void PrintShaderProgramLog(GLuint program);

}; // class Shader

} // namespace dusk

#endif // DUSK_SHADER_HPP
