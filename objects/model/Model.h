#ifndef B_OGL_ENGINE_MODEL_H
#define B_OGL_ENGINE_MODEL_H

#include <string>
#include <vector>
#include <set>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Mesh.h"
#include "../../base/game/Logger.h"
#include "../../base/gl/Shader.h"
#include "../../defs.h"

class Model {
public:
    std::set<Texture> textures;

    static void createFromPath(Model *model, const std::string &name);

    glm::vec3 position;
    glm::vec3 scale;

    void draw(const Shader &shader) const;

private:
    std::string directory;

    std::vector<Mesh> meshes;

    void processNode(aiNode *pNode, const aiScene *pScene);

    void processMesh(aiMesh *pMesh, const aiScene *pScene);

    void loadTextures(std::vector<Texture> *pTextures, aiMaterial *pMat, aiTextureType aiType, Texture::Type type);

    static uint loadTextureFromFile(const std::string &path);
};


#endif //B_OGL_ENGINE_MODEL_H
