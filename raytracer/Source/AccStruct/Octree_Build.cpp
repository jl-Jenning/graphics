#include "Octree.h"
#include "Config.h"

#ifdef DEBUG
    #include <iostream>
    #include <glm/ext.hpp>
#endif

#define LESSTHAN(v1,scale) \
    (v1[0] * scale < 1.f + EPSILON && \
            v1[1] * scale < 1.f + EPSILON && \
            v1[2] * scale < 1.f + EPSILON) 

const static vec3 offsets[8] = { vec3(1,1,1),
                            vec3(-1,1,1),
                            vec3(1,-1,1),
                            vec3(-1,-1,1),
                            vec3(1,1,-1),
                            vec3(-1,1,-1),
                            vec3(1,-1,-1),
                            vec3(-1,-1,-1)};

Octree::Octree()
{
}

Octree::Octree(vector<Object *> objects, BoundingVolume bv, Light l, BVH * bvh)
{
    // Set up root tree.
    this->boxHalfSize = (bv.max - bv.min);
    this->centre = bv.min + boxHalfSize;
    this->occlusion = 0;
    this->directLight = vec3(0);
    this->colour = vec3(0);
    this->makeKids(objects, l, bvh, 0.); 
}

Octree::Octree(vector<Object *> objects, vec3 center, vec3 boxhalfsize, int depth, Light l, BVH * bvh)
{
    // Set up root tree.
    this->boxHalfSize = boxhalfsize;
    this->centre = center;
    this->occlusion = 0;
    this->colour = vec3(0);
    this->directLight = vec3(0);
    this->makeKids(objects, l, bvh, depth); 
}

void Octree::makeKids(vector<Object *> objects, Light l, BVH* bvh, int depth)
{
    if (toDivide(objects) && depth < OCT_DEPTH)
    {
        this->type = NODE;
        this->children = new Octree[8];
        for (int i = 0; i < 8; i++)
        {
            this->children[i] = Octree(objects,
                    offsets[i] * this->boxHalfSize / 2.f + this->centre,
                    this->boxHalfSize/2.f,
                    depth+1,
                    l,
                    bvh);
            this->directLight += 1.f/8.f * this->children[i].directLight;
            this->occlusion += 1.f/8.f * this->children[i].occlusion;
            this->colour += 1.f/8.f * this->children[i].colour;
       }
    }
    else if (depth < OCT_DEPTH)
    {
        this->type = EMPTY;
    }
    else
    {
        this->type = LEAF;
        
        // Calculate occlusion
        Intersection i;
        i.distance = 20;
        vec3 dir = vec3(l.position) - this->centre;
        if (bvh->collision(Ray(vec4(this->centre,0), glm::normalize(dir)), i))
        {
            this->directLight = DirectLight(vec4(this->centre,0), vec3(i.normal), l);
            if (i.distance > glm::l2Norm(dir) && i.distance > EPSILON)
            {
                this->occlusion = 0;
            }
            else
            {
                this->occlusion = 1;
            }
            this->colour = i.colour;
        }
        else
        {
            this->occlusion = 0;
        }
    }
}

bool Octree::toDivide(vector<Object *> objects)
{
    Intersection inter;
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->boxOverlap(this->centre, this->boxHalfSize, inter))
        {
            this->colour = inter.colour;
            return true;
        }
    }
    return false;
}

bool Octree::collision(Ray r, Intersection &inter)
{
    bool didColide = false;
    vec3 min, max;
    max = this->centre + this->boxHalfSize;
    min = this->centre - this->boxHalfSize;
    if (this->type == LEAF)
    {
        max = this->centre + this->boxHalfSize;
        min = this->centre - this->boxHalfSize;
        if(IntersectBoundingVolume(r, BoundingVolume(min,max)))
        {
            float dist = glm::l2Norm(vec3(r.initial), this->centre);
            if (dist < inter.distance)
            {
                inter.position = vec4(this->centre,0);
                inter.colour = this->colour;
                inter.distance = dist;
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (this->type == EMPTY)
    {
        return false;
    }
    else if (this->type == NODE)
    {
        if (IntersectBoundingVolume(r, BoundingVolume(min,max)))
        {
            for (int i = 0; i < 8; i ++)
            {
                didColide |= (this->children[i].collision(r, inter));
            }
        }
    }
    return didColide;
}
