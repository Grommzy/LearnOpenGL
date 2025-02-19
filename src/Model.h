#ifndef MODEL_H
#define MODEL_H

#include <assimp/scene.h>

#include <string>
#include <vector>

class Shader;
class Mesh;
class Texture;

class Model
{
public:
    Model(char* path);

    void Draw(Shader& shader);

private:
    void LoadModel(std::string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

private:
    // Model Data.
    std::vector<Mesh> meshes;
    std::string directory;
};

#endif // MODEL_H