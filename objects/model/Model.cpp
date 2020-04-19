#include <chrono>
#include "Model.h"


int Model::totalCreation = 0;

Mesh Model::cubeMesh;

void Model::draw(const Shader &shader) const {
    auto model = orientation;

    model = glm::translate(glm::mat4(1.0f), position) * model;

    model = glm::scale(model, size);

    shader.setMat4("model", model);

    for (const auto &mesh: meshes) {
        mesh.draw(shader);
    }
}

void Model::drawHighlighted(const Shader &shader) {
    auto tmpScale = size;

    size *= 1.05f;

    draw(shader);

    size = tmpScale;
}

bool Model::intersect(IntersectionRecord *record, Ray ray, const float &tBest) {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    auto rot = glm::mat3(inverseOrientation);

    //Bring ray into model space
    ray.point = ray.point - position;

    //Apply inverse rotation
    ray.point = rot * ray.point;
    ray.direction = rot * ray.direction;

    //Reconstruct the ray
    ray = {ray.point, ray.direction};

    glm::vec3 bounds[2] = {collisionBox[0] * size,
                           collisionBox[1] * size};

    tmin = (bounds[ray.sign[0]].x - ray.point.x) * ray.invdir.x;
    tmax = (bounds[1 - ray.sign[0]].x - ray.point.x) * ray.invdir.x;
    tymin = (bounds[ray.sign[1]].y - ray.point.y) * ray.invdir.y;
    tymax = (bounds[1 - ray.sign[1]].y - ray.point.y) * ray.invdir.y;

    if ((tmin > tymax) || (tymin > tmax)) {
        return false;
    }
    if (tymin > tmin) {
        tmin = tymin;
    }
    if (tymax < tmax) {
        tmax = tymax;
    }

    tzmin = (bounds[ray.sign[2]].z - ray.point.z) * ray.invdir.z;
    tzmax = (bounds[1 - ray.sign[2]].z - ray.point.z) * ray.invdir.z;

    if ((tmin > tzmax) || (tzmin > tmax)) {
        return false;
    }

    if (tzmin > tmin) {
        tmin = tzmin;
    }
    if (tzmax < tmax) {
        tmax = tzmax;
    }

    record->t = tmin;

    return tmin < tBest;
}

void Model::rotate(const float &dx, const float &dy, const float &dz) {
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

void Model::translate(const glm::vec3 &direction) {
    position += direction;
}

void Model::scale(const float &s) {
    size *= s;
}

void Model::scale(const glm::vec3 &sv) {
    size *= sv;
}

void Model::createEditOverlay(bool *p_open) {
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Model Edit", p_open, ImGuiWindowFlags_MenuBar)) {
        ImGui::BeginChild("Position", ImVec2(300, 100), true);
        ImGui::InputFloat("Position x", &position.x, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Position y", &position.y, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Position z", &position.z, 0.01f, 1.0f, "%.3f");
        ImGui::EndChild();

        ImGui::BeginChild("Scaling", ImVec2(300, 100), true);
        ImGui::InputFloat("Size x", &size.x, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Size y", &size.y, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Size z", &size.z, 0.01f, 1.0f, "%.3f");
        ImGui::EndChild();

        ImGui::BeginChild("Rotation", ImVec2(300, 100), true);
        ImGui::InputFloat("x", &roll, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("y", &yaw, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("z", &pitch, 0.01f, 1.0f, "%.3f");
        ImGui::EndChild();

        rotate(0, 0, 0);
    }

    ImGui::End();
}

std::string Model::dump() const {

    std::string result = directory + " " + std::to_string(position.x) + " " + std::to_string(position.y) + " "
                         + std::to_string(position.z) + " " + std::to_string(size.x) + " "
                         + std::to_string(size.y) + " " + std::to_string(size.z) + " " + std::to_string(roll) + " "
                         + std::to_string(yaw) + " " + std::to_string(pitch);
    return result;
}

glm::fquat Model::quaternionRotation(const glm::fquat &quat, glm::vec3 axis, const float &rad) {
    axis = axis * sinf(rad);

    float scalar = cosf(rad);

    glm::fquat offset(scalar, axis.x, axis.y, axis.z);

    return quat * offset;
}

void Model::drawCollisionBox(const Shader &shader) {
    auto tmpScale = size;
    auto tmpPosition = position;
    size *= (collisionBox[1] - collisionBox[0]) / 2.0f;
    position += tmpScale * (collisionBox[1] + collisionBox[0]) / 2.0f;

    auto model = orientation;

    model = glm::translate(glm::mat4(1.0f), position) * model;

    model = glm::scale(model, size);

    shader.setMat4("model", model);

    cubeMesh.draw(shader);

    size = tmpScale;
    position = tmpPosition;
}

