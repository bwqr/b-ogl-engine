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
#include "../../base/struct/Ray.h"
#include "../../base/struct/IntersectionRecord.h"
#include "../../base/overlay/ImGuiOverlay.h"

class Model {
public:
    static int totalCreation;
    int id;
    std::set<Texture> textures;

    std::string directory;

    Model() { id = totalCreation++; }

    static void createFromPath(Model *model, const std::string &name);

    void draw(const Shader &shader) const;

    void drawHighlighted(const Shader &shader);

    bool intersect(IntersectionRecord *record, const Ray &ray, const float &tBest);

    void rotate(const float &dx, const float &dy);

    void translate(const glm::vec3 &direction);

    void scale(const float &s);

    void scale(const glm::vec3 &sv);

    void createEditOverlay(bool *p_open);

    std::string dump() const;

private:
    glm::mat3 xOrientation = {};
    glm::mat3 yOrientation = {};

    glm::vec3 position = {0, 0, 0};
    glm::vec3 size = {1, 1, 1};

    glm::vec3 collusionBox[2];

    float yaw = 0;
    float pitch = 0;

    std::vector<Mesh> meshes;

    void processNode(aiNode *pNode, const aiScene *pScene);

    void processMesh(aiMesh *pMesh, const aiScene *pScene);

    void loadTextures(std::vector<Texture> *pTextures, aiMaterial *pMat, aiTextureType aiType, Texture::Type type);

    static uint loadTextureFromFile(const std::string &path);
};


#endif //B_OGL_ENGINE_MODEL_H
