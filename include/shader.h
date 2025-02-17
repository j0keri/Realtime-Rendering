#ifndef SHADER_H
#define SHADER_H

// Include glad to get all the required OpenGL headers
#include <glad/glad.h>
#include <string>


class Shader
{
public:
    // The shader program ID
    unsigned int ID;
    // Constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // Activate the shader
    void use();
    // Utility uniform functions
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
    void setVec4f(const std::string &name, float x, float y, float z, float w) const;
};
  
#endif