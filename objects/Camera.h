#ifndef GL_HELLO_WORLD_CAMERA_H
#define GL_HELLO_WORLD_CAMERA_H

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <chrono>

#include "../base/struct/Extent2D.h"

class Camera {
public:
    struct {
        glm::mat4 viewProjection;
    } ubo;

    glm::mat4 view;
    glm::mat4 proj;

    Camera() = default;

    Camera(const glm::vec3 &_eye, const glm::vec3 &_viewPlaneVector);

    void rotate(const float &dx, const float &dy);

    void translate(const float &dx, const float &dy);

    void updateUBO();

    void resizeCallback(Extent2D windowExtent);

    void toggleFreeLook();

private:
    bool freeLook = false;

    glm::vec3 eye;
    glm::vec3 viewPlaneVector;

    float aspect = 1;
    float yaw = 0;
    float pitch = 0;

    glm::vec3 orientedViewPlaneVector;
    glm::vec3 orientedUpVector;

    glm::vec3 upVector = {0.0f, 1.0f, 0.0f};

    glm::mat3 xOrientation = {};
    glm::mat3 yOrientation = {};

    static glm::fquat quaternionRotation(const glm::fquat &quat, glm::vec3 axis, const float &rad);
};


#endif //GL_HELLO_WORLD_CAMERA_H
