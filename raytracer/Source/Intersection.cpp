#include <glm/glm.hpp>
#include "Intersection.h"
#include <vector>
#include "Triangle.h"

using namespace std;
using glm::vec3;
using glm::vec4;

vec3 solveLinearEq(Triangle triangle, Ray r);

bool ClosestIntersection(vec4 start, vec4 dir, const vector<Triangle> &triangles, Intersection &closestIntersection){
    bool intersectionFound =  false;
    Ray ray(start, dir);
    
    for(int i = 0; i < triangles.size(); i++){
        vec3 x_value = solveLinearEq(triangles[i],ray);
        if(x_value.y >= 0 && x_value.z >= 0 && x_value.y + x_value.z <= 1 && x_value.x > EPSILON){
            //Valid Intersection found
            intersectionFound = true;
            if(x_value.x < closestIntersection.distance){
                closestIntersection.position = start + x_value.x * dir;
                closestIntersection.distance = x_value.x;
                closestIntersection.triangleIndex = i;
            }
        }
    }
    return intersectionFound;
}

vec3 solveLinearEq(Triangle triangle, Ray r)
{
    vec4 v0 = triangle.v0;
    vec4 v1 = triangle.v1;
    vec4 v2 = triangle.v2;
    vec3 s = glm::vec3(r.initial.x, r.initial.y, r.initial.z);
    vec3 d = glm::vec3(r.direction.x, r.direction.y, r.direction.z);

    vec3 e1 = vec3(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
    vec3 e2 = vec3(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);
    vec3 b = vec3(s.x-v0.x,s.y-v0.y,s.z-v0.z);

    mat3 A( -d, e1, e2 );
    //if (determinant(A) != 0)
    //{
        return glm::inverse( A ) * b;
    //}
    //return vec3(0,0,0);
}