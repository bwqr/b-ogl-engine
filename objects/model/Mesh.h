#ifndef B_OGL_ENGINE_MESH_H
#define B_OGL_ENGINE_MESH_H

#include <vector>
#include <glad/glad.h>
#include "Vertex.h"
#include "Texture.h"
#include "../../base/gl/Shader.h"
#include "Material.h"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
    Material diffuseMaterial;

    void draw(const Shader &shader) const;

    void build();
private:
    uint VAO;
    uint VBO;
    uint EBO;

};


#endif //B_OGL_ENGINE_MESH_H
