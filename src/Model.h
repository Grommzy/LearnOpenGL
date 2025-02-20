#ifndef MODEL_H
#define MODEL_H

#include <assimp/scene.h>

#include <string>
#include <vector>

class Shader;
#include "Mesh.h"

class Model
{
public:
    Model(std::string const &filePath);

    void Draw(Shader& shader);

private:
    void LoadModel(std::string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char* filePath, const std::string& directory);

private:
    // Model Data.
    std::vector<Mesh> meshes;
    std::vector<Texture> loadedTextures;
    std::string directory;
};

#endif // MODEL_H