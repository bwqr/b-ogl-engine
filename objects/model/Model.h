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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

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
    class ModelHandler {
    public:
        ModelHandler(Model *_model);

        static void loadFromPath(Model *model, const std::string &name);

    private:
        Model *model;

        void processMesh(Mesh *mesh, aiMesh *aiMesh, const aiScene *aiScene);

        void processNode(aiNode *aiNode, const aiScene *aiScene);

        void loadTextures(std::vector<Texture> *pTextures, aiMaterial *pMat, aiTextureType aiType, Texture::Type type);

        uint loadTextureFromFile(const std::string &path);
    };

    static int totalCreation;
    int id;
    std::set<Texture> textures;

    std::string directory;

    Model() : modelHandler(this) {
        id = totalCreation++;
    }

    void draw(const Shader &shader) const;

    void drawHighlighted(const Shader &shader);

    bool intersect(IntersectionRecord *record, const Ray &ray, const float &tBest);

    void rotate(const float &dx, const float &dy, const float &dz);

    void translate(const glm::vec3 &direction);

    void scale(const float &s);

    void scale(const glm::vec3 &sv);

    void createEditOverlay(bool *p_open);

    std::string dump() const;

private:
    ModelHandler modelHandler;

    glm::mat4 xOrientation = {};
    glm::mat4 yOrientation = {};
    glm::mat4 zOrientation = {};

    glm::vec3 position = {0, 0, 0};
    glm::vec3 size = {1, 1, 1};

    float roll = 0;
    float yaw = 0;
    float pitch = 0;

    std::array<glm::vec3, 2> collisionBox = {glm::vec3(0, 0, 0),
                                             glm::vec3(0,  0,  0)};

    std::vector<Mesh> meshes;


    static glm::fquat quaternionRotation(const glm::fquat &quat, glm::vec3 axis, const float &rad);
};


#endif //B_OGL_ENGINE_MODEL_H
