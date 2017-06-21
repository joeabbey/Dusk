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

    static std::unique_ptr<Shader> Parse(nlohmann::json & data);

    Shader(const std::vector<std::string>& data, const std::vector<FileInfo>& files);
    virtual ~Shader();

    void Bind();

    GLuint GetGLProgram() const { return _glProgram; }

    GLint GetUniformLocation(const std::string& name);

    void BindData(const std::string& name);

    static void AddData(const std::string& name, void * data, size_t size)
        { UpdateData(name, data, size); }

    static void UpdateData(const std::string& name, void * data, size_t size);

private:


    struct ShaderData
    {
        GLuint glUBO;
        size_t size;
        int index;
    };

    static std::unordered_map<std::string, ShaderData> _DataRecords;
    static int _MaxDataIndex;

    std::vector<FileInfo> _files;

    std::vector<std::string> _boundData;
    GLuint _glProgram;

    bool LoadProgram();
    GLuint LoadShader(const std::string& filename, GLuint type);

    static bool LoadFile(const std::string& filename, std::string& buffer);

    static void PrintShaderLog(GLuint shader);
    static void PrintShaderProgramLog(GLuint program);

}; // class Shader

} // namespace dusk

#endif // DUSK_SHADER_HPP
