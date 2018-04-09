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
    this->boxHalfSize = (bv.max - bv.min) / 2.f  * (1+EPSILON);
    this->centre = bv.min + boxHalfSize;
    this->voxel = new tex_t();
    this->voxel->occ = 0;
    this->voxel->colour = vec3(0);
    this->makeKids(objects, l, bvh, 0.); 
}

Octree::Octree(vector<Object *> objects, vec3 center, vec3 boxhalfsize, int depth, Light l, BVH * bvh)
{
    // Set up root tree.
    this->boxHalfSize = boxhalfsize * (1 + EPSILON);
    this->centre = center;
    this->voxel = new tex_t();
    this->voxel->occ = 0;
    this->voxel->colour = vec3(0);
    this->makeKids(objects, l, bvh, depth); 
}

void Octree::updateTexture()
{
    float count = 0;
    for (int i = 0; i < 8; i++)
    {
        if (this->children[i].voxel != NULL && this->children[i].type != EMPTY)
        {
            this->voxel->occ += this->children[i].voxel->occ;
            this->voxel->colour += this->children[i].voxel->colour;
            count = count + 1;
        }
    }
    if (count >0)
    {
        this->voxel->occ /= 8;
        this->voxel->colour /= 8;
    }
}

void Octree::makeKids(vector<Object *> objects, Light l, BVH* bvh, int depth)
{
    if (this->toDivide(objects, l))
    {
        if (depth < OCT_DEPTH)
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
            }
            this->updateTexture();
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
                if (i.distance > glm::l2Norm(dir) && i.distance > EPSILON)
                {
                    this->voxel->occ = 0;
                }
                else
                {
                    this->voxel->occ = 1;
                }
            }
            else
            {
                this->voxel->occ = 0;
            }
        }

    }
    else 
    {
        this->type = EMPTY;
    }
   }

bool Octree::toDivide(vector<Object *> objects, Light l)
{
    Intersection inter;
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->boxOverlap(this->centre, this->boxHalfSize, inter))
        {
            this->voxel->colour = inter.colour;
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
    float dist = 0;

    if (this->type == LEAF)
    {
        max = this->centre + this->boxHalfSize;
        min = this->centre - this->boxHalfSize;
        if(IntersectBoundingVolume(r, BoundingVolume(min,max)))
        {
            dist = glm::l2Norm(vec3(r.initial), this->centre);
            if (dist < inter.distance)
            {
                inter.position = vec4(this->centre,0);
                inter.colour = vec3(this->voxel->colour);
                inter.distance = dist;
                return true;
            }
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
