#ifndef MESH_H
#define MESH_H

#include <glm.hpp>

#include <vector>
#include <string>

class Shader;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    unsigned int ID;
    std::string Type;
    std::string Path;
};

class Mesh
{
public:
    // Mesh Data.
    std::vector<Vertex>         vertices;
    std::vector<unsigned int>   indices;
    std::vector<Texture>        textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader &shader);

private:
    void SetupMesh();

private:
    // Render data.
    unsigned int VAO, VBO, EBO;
};

#endif // MESH_H