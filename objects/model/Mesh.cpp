#include "Mesh.h"

void Mesh::draw(const Shader &shader) const {
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

void Mesh::build() {
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
