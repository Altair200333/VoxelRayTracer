#include "Shader.h"

#include "Logger.h"

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    const auto& vertexCode = vertexShader;
    const auto& fragmentCode = fragmentShader;
    const auto& geometryCode = geometryPath;

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // vertex shader
    const unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    const unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // if geometry shader is given, compile geometry shader
    unsigned int geometry = 0;
    if (!geometryPath.empty())
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (!geometryPath.empty())
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (!geometryPath.empty())
        glDeleteShader(geometry);

}
// utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------

void Shader::checkCompileErrors(GLuint shader, const std::string& type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            Logger::log("ERROR::SHADER_COMPILATION_ERROR of type: " + type + '\n' + infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            Logger::log("ERROR::PROGRAM_LINKING_ERROR of type: " + type + '\n' + infoLog);
        }
    }
}