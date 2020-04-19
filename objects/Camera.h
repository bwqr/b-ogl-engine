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
#include "../base/struct/Ray.h"

class Camera {
public:
    Camera() = default;

    Camera(const glm::vec3 &_eye, const glm::vec3 &_viewPlaneVector);

    void rotate(const float &dx, const float &dy);

    void translate(const float &dx, const float &dy);

    Ray generateRay(float u, float v);

    glm::vec3 getTranslateDirection(const float &dx, const float &dy);

    Ray getCenterRay();

    glm::mat4 getViewProjectionMatrix();

    void resizeCallback(Extent2D windowExtent);

    void toggleFreeLook();

private:
    glm::mat4 view;
    glm::mat4 proj;

    bool freeLook = false;

    glm::vec3 eye;
    glm::vec3 viewPlaneVector;
    glm::vec3 upVector = {0.0f, 1.0f, 0.0f};

    glm::vec3 orientedUpVector;
    glm::vec3 orientedViewPlaneVector;

    glm::mat3 xOrientation = {};
    glm::mat3 yOrientation = {};

    float aspect = 1;
    float fovy = 60.0f;
    
    float yaw = 0;
    float pitch = 0;

    float translateSensitivity = 1.7;

    static glm::fquat quaternionRotation(const glm::fquat &quat, glm::vec3 axis, const float &rad);

    void updateUBO();

};


#endif //GL_HELLO_WORLD_CAMERA_H
