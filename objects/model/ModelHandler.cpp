#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../../thirdparty/stb/stb_image.h"

void Model::ModelHandler::processMesh(Mesh *mesh, aiMesh *aiMesh, const aiScene *aiScene) {
    for (size_t i = 0; i < aiMesh->mNumVertices; i++) {
        mesh->vertices.emplace_back();

        auto &vertex = mesh->vertices[mesh->vertices.size() - 1];

        vertex.position = {aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z};

        model->collisionBox[0].x = std::min(model->collisionBox[0].x, vertex.position.x);
        model->collisionBox[0].y = std::min(model->collisionBox[0].y, vertex.position.y);
        model->collisionBox[0].z = std::min(model->collisionBox[0].z, vertex.position.z);

        model->collisionBox[1].x = std::max(model->collisionBox[1].x, vertex.position.x);
        model->collisionBox[1].y = std::max(model->collisionBox[1].y, vertex.position.y);
        model->collisionBox[1].z = std::max(model->collisionBox[1].z, vertex.position.z);


        if (aiMesh->mNormals != nullptr) {
            vertex.normal = {aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z};
        }


        if (aiMesh->mTextureCoords[0] != nullptr) {
            vertex.texCoord = {aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y};
        } else {
            vertex.texCoord = {0, 0};
        }
    }

    for (size_t i = 0; i < aiMesh->mNumFaces; i++) {
        auto &face = aiMesh->mFaces[i];

        for (size_t j = 0; j < face.mNumIndices; j++) {
            mesh->indices.push_back(face.mIndices[j]);
        }
    }

    auto *pMaterial = aiScene->mMaterials[aiMesh->mMaterialIndex];
    aiColor4D val;
    pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, val);

    mesh->diffuseMaterial.value = {val.r, val.g, val.b, val.a};

    loadTextures(&mesh->textures, pMaterial, aiTextureType_DIFFUSE, Texture::DIFFUSE_TEXTURE);

    mesh->build();
}

void Model::ModelHandler::processNode(aiNode *aiNode, const aiScene *aiScene) {
    for (size_t i = 0; i < aiNode->mNumMeshes; i++) {
        model->meshes.emplace_back();
        auto &mesh = model->meshes[model->meshes.size() - 1];

        aiMesh *aiMesh = aiScene->mMeshes[aiNode->mMeshes[i]];
        processMesh(&mesh, aiMesh, aiScene);
    }

    for (size_t i = 0; i < aiNode->mNumChildren; i++) {
        processNode(aiNode->mChildren[i], aiScene);
    }
}

void Model::ModelHandler::loadTextures(std::vector<Texture> *pTextures, aiMaterial *pMat, aiTextureType aiType,
                                       Texture::Type type) {
    for (size_t i = 0; i < pMat->GetTextureCount(aiType); i++) {
        aiString path;
        pMat->GetTexture(aiType, i, &path);

        Texture texture = {};
        texture.path = std::string(path.data);

        auto loadedTexture = model->textures.find(texture);
        if (loadedTexture != model->textures.end()) {
            pTextures->push_back(*loadedTexture);
            continue;
        }

        texture.type = type;
        texture.id = loadTextureFromFile(MODELS_DIR + model->directory + texture.path);

        model->textures.insert(texture);
        pTextures->push_back(texture);
    }
}

uint Model::ModelHandler::loadTextureFromFile(const std::string &path) {
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

Model::ModelHandler::ModelHandler(Model *_model) {
    model = _model;
}

void Model::ModelHandler::loadFromPath(Model *model, const std::string &name) {
    Assimp::Importer assimpImporter;

    const aiScene *scene = assimpImporter
            .ReadFile(MODELS_DIR + name + "/model.obj", aiProcess_Triangulate | aiProcess_FlipUVs);

    if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
        Logger::error(assimpImporter.GetErrorString(), Logger::MODEL_LOAD_FAILED);

        return;
    }

    model->directory = name;
    model->modelHandler.processNode(scene->mRootNode, scene);

    if (cubeMesh.vertices.size() > 0) {
        return;
    }

    Mesh mesh;

    float vertices[6 * 6 * 3] = {
            // positions
            -1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,

            1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,

            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,

            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,

            1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,

            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,

            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
    };

    for (size_t i = 0; i < sizeof(vertices) / sizeof(float); i += 3) {
        Vertex vert = {};
        vert.position = {vertices[i], vertices[i + 1], vertices[i + 2]};
        mesh.vertices.push_back(vert);
        mesh.indices.push_back(i);
        mesh.indices.push_back(i + 1);
        mesh.indices.push_back(i + 2);
    }

    mesh.build();

    cubeMesh = mesh;
}
