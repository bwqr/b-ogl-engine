#ifndef B_OGL_ENGINE_RAY_H
#define B_OGL_ENGINE_RAY_H

#include <glm/vec3.hpp>

class Ray {
public:
    Ray(const glm::vec3 &_point, const glm::vec3 &_direction) : point(_point), direction(_direction) {
        invdir.x = 1 / _direction.x;
        invdir.y = 1 / _direction.y;
        invdir.z = 1 / _direction.z;

        sign[0] = (invdir.x < 0);
        sign[1] = (invdir.y < 0);
        sign[2] = (invdir.z < 0);
    }

    glm::vec3 invdir;
    bool sign[3];
    glm::vec3 point;
    glm::vec3 direction;

    Ray() = default;

    glm::vec3 move(float t) const {
        return point + direction * t;
    }

    float findT(float z) const {
        return (z - point.z) / direction.z;
    }
};

#endif //B_OGL_ENGINE_RAY_H
