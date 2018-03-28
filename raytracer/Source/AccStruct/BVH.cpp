#include "BVH.h"
#include "Config.h"
#include <assert.h>

#ifdef DEBUG
#include <iostream>
#include "glm/ext.hpp"
#endif

using namespace std;

BoundingVolume computeBoundingVolume(vector<Object *> objects);
vector<vector<Object *> > partitionObject(vector<Object *> objects);
bool IntersectRayBoundingVolume(Ray r,
        BoundingVolume bv);

BVH::BVH()
{
    left = NULL;
    right = NULL;
}

BVH::BVH(vector<Object *> objects)
{
    assert(objects.size() > 0);
    bv = computeBoundingVolume(objects);
    
    if (objects.size() == 1)
    {
        isLeaf = true;
        object = objects[0];
    }
    else
    {
        isLeaf = false;
        vector<vector<Object *> > partitioning = partitionObject(objects);        
        left = new BVH(partitioning[0]);
        right = new BVH(partitioning[1]);
    }
}

bool collision(BVH bvh, Ray r, Intersection &closestI)
{
    bool intersectionFound = false;
    if (IntersectRayBoundingVolume(r, bvh.bv))
    {
        if (bvh.isLeaf == true)
        {
           intersectionFound |= bvh.object->intersection(r, closestI); 
        }
        else
        {
            intersectionFound |= collision(*bvh.left, r, closestI);
            intersectionFound |= collision(*bvh.right, r, closestI);
        }
    }
    else
    {
        return false;
    }
    return intersectionFound;
}

vector<vector<Object *> > partitionObject(vector<Object *> objects)
{
    // Naive median parititoning.
    //
    //
    int index = objects.size()/2;
    vector<Object*> left, right;
    vector<vector<Object*> >  result = vector<vector<Object*> >();
    for (int i = 0; i < index; i++)
    {
        left.push_back(objects[i]);
    }
    for (int i = index; i < objects.size(); i++)
    {
        right.push_back(objects[i]);
    }
    result.push_back(left);
    result.push_back(right);
    return result;
}


BoundingVolume computeBoundingVolume(const vector<Object *> objects)
{
    vec3 max = vec3(std::numeric_limits<float>::min());
    vec3 min = vec3(std::numeric_limits<float>::max());

    for (int i = 0; i < objects.size(); i ++)
    {
        if (objects[i]->bv.min.x < min.x) { min.x=objects[i]->bv.min.x; }
        if (objects[i]->bv.min.y < min.y) { min.y=objects[i]->bv.min.y; }
        if (objects[i]->bv.min.z < min.z) { min.z=objects[i]->bv.min.z; }
        if (objects[i]->bv.max.x > max.x) { max.x=objects[i]->bv.max.x; }
        if (objects[i]->bv.max.y > max.y) { max.y=objects[i]->bv.max.y; }
        if (objects[i]->bv.max.z > max.z) { max.z=objects[i]->bv.max.z; }
        //cout << glm::to_string(max) << endl;
    }
    //cout <<  endl;
    return BoundingVolume(min,max);
}

bool IntersectRayBoundingVolume(Ray r, BoundingVolume bv)
{
    vec3 tmin, tmax;
    for (int i = 0; i < 3; i++)
    {
        if (r.direction[i] >= 0)
        {
            tmin[i] = (bv.min[i] - r.initial[i]) / r.direction[i];
            tmax[i] = (bv.max[i] - r.initial[i]) / r.direction[i];
        }
        else
        {
            tmin[i] = (bv.max[i] - r.initial[i]) / r.direction[i];
            tmax[i] = (bv.min[i] - r.initial[i]) / r.direction[i];
        }
    }

    if ((tmin[0] > tmax[1]) || (tmin[1] > tmax[0] )) {return false;}
    if (glm::max(tmin[0], tmin[1]) > tmax[2] || tmin[2] > glm::min(tmax[0], tmax[1]))
    {
     return false;
    }
    return true;
}
