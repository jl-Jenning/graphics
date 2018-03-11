#ifndef CLOSEST_INTERSECTION_H
#define CLOSEST_INTERSECTION_H

#include <glm/glm.hpp>
#include <vector>
#include "Object.h"
#include "BVH.h"
#include "Camera.h"

using namespace std;
// Intersection
struct Intersection
{
    glm::vec4 position;
    glm ::vec3 colour;
    float distance;
    vec4 normal;
};

bool ClosestIntersection(vec4 start,
        vec4 dir,
        const vector<Object> &objects,
        Intersection &closestIntersection);

bool IntersectRayBoundingVolume(vec4 start,
        vec4 dir,
        BoundingVolume bv);

#endif
