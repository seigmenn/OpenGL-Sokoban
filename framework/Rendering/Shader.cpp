#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Shader.h"

    // constructor reads and builds the shader
    Shader::Shader(const std::string vertexShader, const std::string fragmentShader)
    {
        ID = compileShader(vertexShader,fragmentShader);
    }

    Shader::~Shader() {
    }

    // use/activate the shader
    void Shader::use() {
        glUseProgram(ID);
    }


    // utility uniform functions
    void Shader::setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void Shader::setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
        
    void Shader::setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::SetUniform4f(const std::string& name, glm::vec4 v) {
        glUniform4f(GetUniformLocation(name.c_str()), v[0], v[1], v[2], v[3]);
    }

    void Shader::SetUniform3f(const std::string& name, glm::vec3 v) {
        glUniform3f(GetUniformLocation(name.c_str()), v[0], v[1], v[2]);
    }

void Shader::SetUniform2f(const std::string& name, glm::vec2 v) {
        glUniform2f(GetUniformLocation(name.c_str()), v.x, v.y);
    }
    void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    int Shader::GetUniformLocation(const std::string& name) {
         int loc = glGetUniformLocation(ID, name.c_str());
         if (loc == -1)
             std::cout << "WARNING: uniform '" << name << "' can't be found!" << std::endl;
         return loc;
    }

GLuint Shader::compileShader(const std::string vertexShader, const std::string &fragmentShader) {
    //Convert shader source code
    auto vertexSrc = vertexShader.c_str();
    auto fragmentSrc = fragmentShader.c_str();

    //Compile vertex shader
    auto vertexShaders = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaders, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShaders);

    GLint isCompiled = 0;
    glGetShaderiv(vertexShaders, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled)
    {
        std::cout << "Vertex\n";
        GLint maxLength = 0;
        glGetShaderiv(vertexShaders, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertexShaders, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.
        std::cout << errorLog.data() << std::endl;
        std::cin.get();
    }

    //Compile fragment shader
    auto fragmentShaders = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaders, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShaders);

    glGetShaderiv(fragmentShaders, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled)
    {
        std::cout << "Fragment\n";
        GLint maxLength = 0;
        glGetShaderiv(fragmentShaders, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragmentShaders, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.
        std::cout << errorLog.data() << std::endl;
        std::cin.get();
    }

    //Create shader program
    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShaders);
    glAttachShader(shaderProgram,fragmentShaders);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShaders);
    glDeleteShader(fragmentShaders);

    return shaderProgram;
}
