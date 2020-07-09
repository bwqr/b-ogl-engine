#ifndef B_OGL_ENGINE_BEZIERMESH_H
#define B_OGL_ENGINE_BEZIERMESH_H

#include <vector>
#include <glad/glad.h>
#include "Vertex.h"
#include "Texture.h"
#include "../../base/gl/Shader.h"
#include "Material.h"

#define ORDER 3

class BezierMesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
    Material diffuseMaterial;

    void draw(const Shader &shader) const;

    void build(const glm::vec3 *controlPoints, const uint32_t controlPointsIndices[][ORDER + 1][ORDER + 1], int patch, int resu, int resv);

private:
    uint VAO;
    uint VBO;
    uint EBO;

    glm::vec3
    compute_position(const glm::vec3 *controlPoints, const uint32_t controlPointsIndices[][ORDER + 1][ORDER + 1], int patch, float u, float v);

    float bernstein_polynomial(int i, int n, float u);

    float binomial_coefficient(int i, int n);

    int factorial(int n);
};


#endif //B_OGL_ENGINE_BEZIERMESH_H
