#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm.hpp>

#include <string>

class Shader
{
public:
    // Constructor
    Shader(const char* vertexPath, const char* fragmentPath);

    // Activate the shader
    void Use();

    // Utility uniform function - Boolean
    void SetBool(const std::string &name, bool value) const;
    
    // Utility uniform function - Int
    void SetInt(const std::string &name, int value) const;
    
    // Utility uniform function - Float
    void SetFloat(const std::string &name, float value) const;

    // Utility uniform function - Vector3
    void SetVec3(const std::string &name, float x, float y, float z) const;
    void SetVec3(const std::string &name, const glm::vec3 &value) const;

    // Retrieve the private ID of the shader
    unsigned int GetID();

private:
    // Utility function for checking shader compilation/linking errors.
    void CheckCompileErrors(unsigned int shader, std::string type);

private:
    unsigned int ID;
};
#endif