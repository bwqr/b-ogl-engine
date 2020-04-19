#include "Camera.h"
#include "../defs.h"

Camera::Camera(const glm::vec3 &_eye, const glm::vec3 &_viewPlaneVector) {
    eye = _eye;
    viewPlaneVector = _viewPlaneVector;
}

void Camera::resizeCallback(Extent2D windowExtent) {
    aspect = windowExtent.width / static_cast<float>(windowExtent.height);
}

void Camera::rotate(const float &dx, const float &dy) {
    const static glm::fquat baseOrientation = {1.0f, 0.0f, 0.0f, 0.0f};

    const static float sensitivity = 45.0f;
    const static float div = 90.0f / sensitivity / 2.0f;
    float fracpart, intpart;

    yaw += dx;
    if (yaw > 180.0f / sensitivity || yaw < -180.0f / sensitivity) {
        fracpart = std::modf(yaw + dx, &intpart);
        yaw = static_cast<int>(intpart) % static_cast<int>(180.0f / sensitivity) + fracpart;
    }

    pitch = std::max(std::min(pitch + dy, div), -div);

    xOrientation = glm::mat3_cast(
            quaternionRotation(baseOrientation, {0, 1, 0}, glm::radians(-yaw * sensitivity)));
    yOrientation = glm::mat3_cast(
            quaternionRotation(baseOrientation, {1, 0, 0}, glm::radians(-pitch * sensitivity)));

    orientedViewPlaneVector = xOrientation * yOrientation * viewPlaneVector;

    orientedUpVector = xOrientation * yOrientation * upVector;

    updateUBO();
}

void Camera::translate(const float &dx, const float &dy) {

    eye += getTranslateDirection(dx, dy);

    updateUBO();
}

glm::fquat Camera::quaternionRotation(const glm::fquat &quat, glm::vec3 axis, const float &rad) {
    axis = axis * sinf(rad);

    float scalar = cosf(rad);

    glm::fquat offset(scalar, axis.x, axis.y, axis.z);

    return quat * offset;
}

void Camera::toggleFreeLook() {
    freeLook = !freeLook;
}

void Camera::updateUBO() {
    view = glm::lookAt(eye, eye + orientedViewPlaneVector, orientedUpVector);
    proj = glm::perspective(glm::radians(fovy), aspect, NEAR_VIEW, FAR_VIEW);
}

Ray Camera::getCenterRay() {
    return {eye + orientedViewPlaneVector, orientedViewPlaneVector};
}

glm::vec3 Camera::getTranslateDirection(const float &dx, const float &dy) {
    glm::mat3 orientation = freeLook ? xOrientation * yOrientation : xOrientation;

    return orientation * glm::vec3(1, 0, 0) * dx * translateSensitivity +
           orientation * glm::vec3(0, 0, -1) * dy * translateSensitivity;
}

glm::mat4 Camera::getViewProjectionMatrix() {
    return proj * view;
}

Ray Camera::generateRay(float u, float v) {

    float a = glm::length(viewPlaneVector);

    auto halfFovy = glm::radians(fovy) / 2;

    float b = a * std::sin(halfFovy) / std::cos(halfFovy);

    auto yDir = b * glm::normalize(orientedUpVector) * 2.0f;

    auto xDir = b * aspect * glm::normalize(glm::cross(orientedViewPlaneVector, orientedUpVector)) * 2.0f;

    glm::vec3 pointInWorldSpace = eye + orientedViewPlaneVector + yDir * (v - .5f) + xDir * (u - .5f);

    return {pointInWorldSpace, pointInWorldSpace - eye};
}
