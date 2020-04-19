#ifndef RENDER_HELLO_WORLD_INTERSECTIONRECORD_H
#define RENDER_HELLO_WORLD_INTERSECTIONRECORD_H


#include <glm/vec3.hpp>

struct IntersectionRecord {
    float t;
    glm::vec3 hitPoint;
    glm::vec3 normal;
    glm::vec3 look;
};

#endif //RENDER_HELLO_WORLD_INTERSECTIONRECORD_H
