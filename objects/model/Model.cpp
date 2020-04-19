#include <chrono>
#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../../thirdparty/stb/stb_image.h"

int Model::totalCreation = 0;

void Model::createFromPath(Model *model, const std::string &name) {

    Assimp::Importer assimpImporter;

    const aiScene *scene = assimpImporter
            .ReadFile(MODELS_DIR + name + "/model.obj", aiProcess_Triangulate | aiProcess_FlipUVs);

    if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
        Logger::error(assimpImporter.GetErrorString(), Logger::MODEL_LOAD_FAILED);

        return;
    }

//    for (size_t i = 0; i < scene->mNumMaterials; i++) {
//        Logger::info("MATERIAL");
//
//        aiColor4D as;
//        scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, as);
//        Logger::info(scene->mMaterials[i]->GetName().C_Str());
//        std::cout << as.r << " " << as.g << " " << as.b << " " << as.a << std::endl;
////        for (size_t j = 0; j < scene->mMaterials[i]->mNumProperties; j++) {
////            Logger::info(scene->mMaterials[i]->mProperties[j]->mKey.data);
////            for (size_t k = 0; k < scene->mMaterials[i]->mProperties[j]->mDataLength; k += 4) {
////                auto data = scene->mMaterials[i]->mProperties[j]->mData;
////            }
////        }
//
//    }

    model->directory = name;
    model->processNode(scene->mRootNode, scene);
}

void Model::draw(const Shader &shader) const {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();

    auto model = glm::translate(glm::mat4(1.0f), position);

    model = glm::rotate(model,
                        std::chrono::duration<float, std::chrono::seconds::period>(
                                currentTime - startTime).count() * glm::radians(0.0f),
                        glm::vec3(1, 0, 0));

    model = glm::scale(model, size);

    shader.setMat4("model", model);

    for (const auto &mesh: meshes) {
        mesh.draw(shader);
    }
}

void Model::processNode(aiNode *pNode, const aiScene *pScene) {
    for (size_t i = 0; i < pNode->mNumMeshes; i++) {
        aiMesh *mesh = pScene->mMeshes[pNode->mMeshes[i]];
        processMesh(mesh, pScene);
    }

    for (size_t i = 0; i < pNode->mNumChildren; i++) {
        processNode(pNode->mChildren[i], pScene);
    }
}

void Model::processMesh(aiMesh *pMesh, const aiScene *pScene) {
    meshes.emplace_back();

    auto &mesh = meshes[meshes.size() - 1];

    for (size_t i = 0; i < pMesh->mNumVertices; i++) {
        mesh.vertices.emplace_back();

        auto &vertex = mesh.vertices[mesh.vertices.size() - 1];

        glm::vec3 vec;

        vertex.position = {pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z};

        if (pMesh->mNormals != nullptr) {
            vertex.normal = {pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z};
        }


        if (pMesh->mTextureCoords[0] != nullptr) {
            vertex.texCoord = {pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y};
        } else {
            vertex.texCoord = {0, 0};
        }
    }

    for (size_t i = 0; i < pMesh->mNumFaces; i++) {
        auto &face = pMesh->mFaces[i];

        for (size_t j = 0; j < face.mNumIndices; j++) {
            mesh.indices.push_back(face.mIndices[j]);
        }
    }

    auto *pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];
    aiColor4D val;
    pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, val);

    mesh.diffuseMaterial.value = {val.r, val.g, val.b, val.a};

    loadTextures(&mesh.textures, pMaterial, aiTextureType_DIFFUSE, Texture::DIFFUSE_TEXTURE);

    mesh.build();
}

void Model::loadTextures(std::vector<Texture> *pTextures, aiMaterial *pMat, aiTextureType aiType, Texture::Type type) {

    for (size_t i = 0; i < pMat->GetTextureCount(aiType); i++) {
        aiString path;
        pMat->GetTexture(aiType, i, &path);

        Texture texture = {};
        texture.path = std::string(path.data);

        auto loadedTexture = textures.find(texture);
        if (loadedTexture != textures.end()) {
            pTextures->push_back(*loadedTexture);
            continue;
        }

        texture.type = type;
        texture.id = loadTextureFromFile(MODELS_DIR + directory + texture.path);

        textures.insert(texture);
        pTextures->push_back(texture);
    }
}

uint Model::loadTextureFromFile(const std::string &path) {
    uint textureId;

    glGenTextures(1, &textureId);

    int width, height, channels;

    auto *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (data != nullptr) {
        GLenum format;

        if (channels == 1) {
            format = GL_RED;
        } else if (channels == 3) {
            format = GL_RGB;
        } else if (channels == 4) {
            format = GL_RGBA;
        } else {
            Logger::error("number of channels are not supported" + std::to_string(channels),
                          Logger::TEXTURE_LOAD_ERROR);
        }

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    } else {
        Logger::error("could not read the texture file " + path, Logger::TEXTURE_LOAD_ERROR);
    }

    stbi_image_free(data);

    return textureId;
}

void Model::drawHighlighted(const Shader &shader) {
    auto tmpScale = size;
    size *= 1.05;

    draw(shader);

    size = tmpScale;
}

bool Model::intersect(IntersectionRecord *record, const Ray &ray, const float &tBest) {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    glm::vec3 bounds[2] = {position - 1.f * size,
                           position + 1.f * size};

    tmin = (bounds[ray.sign[0]].x - ray.point.x) * ray.invdir.x;
    tmax = (bounds[1 - ray.sign[0]].x - ray.point.x) * ray.invdir.x;
    tymin = (bounds[ray.sign[1]].y - ray.point.y) * ray.invdir.y;
    tymax = (bounds[1 - ray.sign[1]].y - ray.point.y) * ray.invdir.y;

    if ((tmin > tymax) || (tymin > tmax)) {
        return false;
    }
    if (tymin > tmin) {
        tmin = tymin;
    }
    if (tymax < tmax) {
        tmax = tymax;
    }

    tzmin = (bounds[ray.sign[2]].z - ray.point.z) * ray.invdir.z;
    tzmax = (bounds[1 - ray.sign[2]].z - ray.point.z) * ray.invdir.z;

    if ((tmin > tzmax) || (tzmin > tmax)) {
        return false;
    }

    if (tzmin > tmin) {
        tmin = tzmin;
    }
    if (tzmax < tmax) {
        tmax = tzmax;
    }

    record->t = tmin;

    return tmin < tBest;
}

void Model::rotate(const float &dx, const float &dy) {
    // NOT IMPLEMENTED
}

void Model::translate(const glm::vec3 &direction) {
    position += direction;
}

void Model::scale(const float &s) {
    size *= s;
}

void Model::scale(const glm::vec3 &sv) {
    size *= sv;
}

void Model::createEditOverlay(bool *p_open) {
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Model Edit", p_open, ImGuiWindowFlags_MenuBar)) {
        ImGui::BeginChild("Position", ImVec2(300, 0), true);
        ImGui::InputFloat("Position x", &position.x, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Position y", &position.y, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Position z", &position.z, 0.01f, 1.0f, "%.3f");
        ImGui::EndChild();

        ImGui::BeginChild("Position", ImVec2(300, 0), true);
        ImGui::InputFloat("Size x", &size.x, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Size y", &size.y, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Size z", &size.z, 0.01f, 1.0f, "%.3f");
        ImGui::EndChild();
    }

    ImGui::End();
}

std::string Model::dump() const {

    std::string result = directory + " " + std::to_string(position.x) + " " + std::to_string(position.y) + " "
                         + std::to_string(position.z) + " " + std::to_string(size.x) + " "
                         + std::to_string(size.y) + " " + std::to_string(size.z);
    return result;
}
