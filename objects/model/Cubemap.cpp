#include "Cubemap.h"

Cubemap::Cubemap(const std::array<std::string, 6> &faces) {
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);

    int width, height, channels;

    for (size_t i = 0; i < faces.size(); i++) {
        auto *data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

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

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
                         data);
        } else {
            Logger::error("could not read the texture file " + faces[i], Logger::TEXTURE_LOAD_ERROR);
        }

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
}

void Cubemap::draw(const Shader &shader) {
    shader.setInt("cubemap", 0);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));

    glBindVertexArray(0);
}
