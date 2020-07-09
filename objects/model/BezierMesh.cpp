#include "BezierMesh.h"

void BezierMesh::draw(const Shader &shader) const {
    uint diffuseNumber = 1;

    for (size_t i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string textureVar;
        switch (textures[i].type) {
            case Texture::DIFFUSE_TEXTURE:
                textureVar = "textureDiffuse" + std::to_string(diffuseNumber++);
                break;
        }

        shader.setInt(textureVar, i);

        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    shader.setVec4("diffuseColor", diffuseMaterial.value);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void BezierMesh::build(const glm::vec3 *controlPoints, const uint32_t controlPointsIndices[][ORDER + 1][ORDER + 1],
                       int patch, int resu, int resv) {
//    cleanup();

    // Build up the vertices
    vertices.resize(resu * resv);
    for (size_t j = 0; j < resu; j++) {
        float u = static_cast<float>(j) / (resu - 1);
        for (size_t k = 0; k < resv; k++) {
            float v = static_cast<float>(k) / (resv - 1);
            // Build vertices
            uint index = j * resv + k;
            vertices[index].position = compute_position(controlPoints, controlPointsIndices, patch, u, v);
        }
    }

    // Build up the indices
    indices.resize((resu - 1) * (resv - 1) * 2 * 3);
    int n = 0;
    for (size_t j = 0; j < resu - 1; j++) {
        for (size_t k = 0; k < resv - 1; k++) {
            // Build indices
            // ABC
            indices[n] = j * resv + k;
            n++;
            indices[n] = j * resv + (k + 1);
            n++;
            indices[n] = (j + 1) * resv + (k + 1);
            n++;
            // CDA
            indices[n] = (j + 1) * resv + (k + 1);
            n++;
            indices[n] = (j + 1) * resv + k;
            n++;
            indices[n] = j * resv + k;
            n++;
        }
    }

    glGenVertexArrays(1, &VAO);
    uint genBuffers[2];
    glGenBuffers(2, genBuffers);
    VBO = genBuffers[0];
    EBO = genBuffers[1];

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoord));

    glBindVertexArray(0);
}

glm::vec3 BezierMesh::compute_position(const glm::vec3 *controlPoints,
                                       const uint32_t controlPointsIndices[][ORDER + 1][ORDER + 1], int patch, float u,
                                       float v) {
    glm::vec3 result = {0, 0, 0};

    for (size_t i = 0; i < ORDER + 1; ++i) {
        float iPoly = bernstein_polynomial(i, ORDER, u);

        for (size_t j = 0; j < ORDER + 1; ++j) {
            float jPoly = bernstein_polynomial(j, ORDER, v);
            result += iPoly * jPoly * controlPoints[controlPointsIndices[patch][i][j]];
        }
    }
    return result;
}

float BezierMesh::bernstein_polynomial(int i, int n, float u) {
    return binomial_coefficient(i, n) * powf(u, i) * powf(1 - u, n - i);
}

float BezierMesh::binomial_coefficient(int i, int n) {
    if (i < 0 || n < 0) {
        throw std::runtime_error("binomial coefficient i or n cannot be less than 0");
    }

    return static_cast<float>(factorial(n)) / (factorial(i) * factorial(n - i));
}

int BezierMesh::factorial(int n) {
    if (n < 0) {
        throw std::runtime_error("factorial n cannot be less than 0");
    }

    int res = 1;

    for (size_t i = n; i > 1; i--) {
        res *= i;
    }

    return res;
}

//void BezierMesh::build(glm::vec3 *controlPoints, uint32_t *controlPointsIndices, int patch, int resu, int resv) {
////    cleanup();
//
//    // Build up the vertices
//    vertices.resize(PATCH_SIZE * resu * resv);
//    for (size_t i = 0; i < PATCH_SIZE; i++) {
//        for (size_t j = 0; j < resu; j++) {
//            float u = static_cast<float>(j) / (resu - 1);
//            for (size_t k = 0; k < resv; k++) {
//                float v = static_cast<float>(k) / (resv - 1);
//                // Build vertices
//                uint index = i * resu * resv + j * resv + k;
//                vertices[index].position = compute_position(i, u, v);
//            }
//        }
//    }
//
//    // Build up the indices
//    indices.resize(PATCH_SIZE * (resu - 1) * (resv - 1) * 2 * 3);
//    int n = 0;
//    for (size_t i = 0; i < PATCH_SIZE; i++) {
//        for (size_t j = 0; j < resu - 1; j++) {
//            for (size_t k = 0; k < resv - 1; k++) {
//                // Build indices
//                // ABC
//                indices[n] = i * resu * resv + j * resv + k;
//                n++;
//                indices[n] = i * resu * resv + j * resv + (k + 1);
//                n++;
//                indices[n] = i * resu * resv + (j + 1) * resv + (k + 1);
//                n++;
//                // CDA
//                indices[n] = i * resu * resv + (j + 1) * resv + (k + 1);
//                n++;
//                indices[n] = i * resu * resv + (j + 1) * resv + k;
//                n++;
//                indices[n] = i * resu * resv + j * resv + k;
//                n++;
//            }
//        }
//    }
//
//    glGenVertexArrays(1, &VAO);
//    uint genBuffers[2];
//    glGenBuffers(2, genBuffers);
//    VBO = genBuffers[0];
//    EBO = genBuffers[1];
//
//    glBindVertexArray(VAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);
//
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
//
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
//
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoord));
//
//    glBindVertexArray(0);
//}
