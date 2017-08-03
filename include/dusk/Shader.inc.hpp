#define SET_UNIFORM(BaseType, Type, Suffix)                                                        \
    void SetUniform(const std::string& name, Type value)                                           \
    {                                                                                              \
        GLint location = (_uniforms.find(name) == _uniforms.end() ? 0 : _uniforms[name].Location); \
        glUniform##Suffix##v(location, 1, (BaseType*)&value);                                      \
    }                                                                                              \
                                                                                                   \
    void SetUniformArray(const std::string& name, Type value, GLsizei count)                       \
    {                                                                                              \
        GLint location = (_uniforms.find(name) == _uniforms.end() ? 0 : _uniforms[name].Location); \
        glUniform##Suffix##v(location, count, (BaseType*)&value);                                  \
    }

#define SET_UNIFORM_MATRIX(Type, Suffix)                                                           \
    void SetUniformMatrix(const std::string& name, Type value)                                     \
    {                                                                                              \
        GLint location = (_uniforms.find(name) == _uniforms.end() ? 0 : _uniforms[name].Location); \
        glUniformMatrix##Suffix##v(location, 1, GL_FALSE, (float*)&value);                         \
    }                                                                                              \
                                                                                                   \
    void SetUniformMatrixArray(const std::string& name, Type value, GLsizei count)                 \
    {                                                                                              \
        GLint location = (_uniforms.find(name) == _uniforms.end() ? 0 : _uniforms[name].Location); \
        glUniformMatrix##Suffix##v(location, count, GL_FALSE, (float*)&value);                     \
    }

SET_UNIFORM(float, float, 1f);
SET_UNIFORM(float, glm::vec2, 2f);
SET_UNIFORM(float, glm::vec3, 3f);
SET_UNIFORM(float, glm::vec4, 4f);

SET_UNIFORM(int, int, 1i);
SET_UNIFORM(int, glm::ivec2, 2i);
SET_UNIFORM(int, glm::ivec3, 3i);
SET_UNIFORM(int, glm::ivec4, 4i);

SET_UNIFORM(unsigned int, unsigned int, 1ui);
SET_UNIFORM(unsigned int, glm::uvec2, 2ui);
SET_UNIFORM(unsigned int, glm::uvec3, 3ui);
SET_UNIFORM(unsigned int, glm::uvec4, 4ui);

SET_UNIFORM_MATRIX(glm::mat2, 2f);
SET_UNIFORM_MATRIX(glm::mat3, 3f);
SET_UNIFORM_MATRIX(glm::mat4, 4f);

#undef SET_UNIFORM
#undef SET_UNIFORM_MATRIX
