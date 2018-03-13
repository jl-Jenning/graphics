#ifndef CLOSEST_INTERSECTION_H
#define CLOSEST_INTERSECTION_H

#include <glm/glm.hpp>
#include <vector>
#include "Triangle.h"

using namespace glm;
// Intersection
struct Intersection
{
    glm::vec4 position;
    glm ::vec3 colour;
    float distance;
    vec4 normal;
    float reflect_ratio;
    float refract_ratio;
    float ior;
};

#endif
