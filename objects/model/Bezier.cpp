#include "Bezier.h"

void Bezier::build() {
    // Build teapot for all meshes, normally one mesh is expected in the vector
    meshes.clear();

    for (size_t i = 0; i < PATCH_SIZE; i++) {
        BezierMesh mesh;
        mesh.build(teapot_cp, teapot_cp_indices, i, resolution.width, resolution.height);
        mesh.diffuseMaterial.value = glm::vec4({1, 1, 1, 1});
        mesh.diffuseMaterial.value /= i;
        meshes.push_back(mesh);
    }

}

void Bezier::draw(const Shader &shader) const {
    auto model = orientation;

    model = glm::translate(glm::mat4(1.0f), position) * model;

    model = glm::scale(model, size);

    shader.setMat4("model", model);

    for (const auto &mesh: meshes) {
        mesh.draw(shader);
    }
}

void Bezier::rotate(const float &dx, const float &dy, const float &dz) {
    const static glm::fquat baseOrientation = {1.0f, 0.0f, 0.0f, 0.0f};

    roll += dx;

    yaw += dy;

    pitch += dz;

    float intpart;

    if (roll > 360 || roll < -360) {
        roll = std::modf(roll + dx, &intpart) + static_cast<int>(intpart) % 360;
    }

    if (yaw > 360 || yaw < -360) {
        yaw = std::modf(yaw + dy, &intpart) + static_cast<int>(intpart) % 360;
    }

    if (pitch > 360 || pitch < -360) {
        pitch = std::modf(pitch + dz, &intpart) + static_cast<int>(intpart) % 360;
    }

    auto xOrientation = glm::mat3_cast(
            quaternionRotation(baseOrientation, {1, 0, 0}, glm::radians(roll) / 2));
    auto yOrientation = glm::mat3_cast(
            quaternionRotation(baseOrientation, {0, 1, 0}, glm::radians(yaw) / 2));
    auto zOrientation = glm::mat3_cast(
            quaternionRotation(baseOrientation, {0, 0, 1}, glm::radians(pitch) / 2));

    orientation = xOrientation * yOrientation * zOrientation;
    inverseOrientation = glm::inverse(orientation);
}

void Bezier::translate(const glm::vec3 &direction) {
    position += direction;
}

void Bezier::scale(const float &s) {
    size *= s;
}

void Bezier::scale(const glm::vec3 &sv) {
    size *= sv;
}


glm::fquat Bezier::quaternionRotation(const glm::fquat &quat, glm::vec3 axis, const float &rad) {
    axis = axis * sinf(rad);

    float scalar = cosf(rad);

    glm::fquat offset(scalar, axis.x, axis.y, axis.z);

    return quat * offset;
}

void Bezier::increaseResolution() {
    resolution.width++;
    resolution.height++;
    build();
}

void Bezier::decreaseResolution() {
    if (resolution.width == 1 || resolution.height == 1) {
        return;
    }

    resolution.width--;
    resolution.height--;
    build();
}

void Bezier::setResolution(Extent2D res) {
    if (res.width < 1 || res.height < 1) {
        return;
    }

    resolution = res;
}
