#pragma once

#include<scene/geometry/boundingbox.h>
#include<raytracing/ray.h>
#include<raytracing/intersection.h>

class Geometry;
class Intersection;
class Triangle;

class BVHNode
{
public:
    BVHNode();
    BVHNode(QList<Geometry*> &objects, int axis);
    BVHNode(QList<Triangle*> &objects, int axis);

    Geometry* obj_enclosed;

    Intersection GetIntersection(Ray r);

    float evaluate_SAH(BoundingBox left, BoundingBox right, int triangles_left, int triangles_right);
    float surface_area(BoundingBox Box);
    BoundingBox MakeBox(QList<Triangle*> objects);

    void Clear();

    BVHNode* left_child;
    BVHNode* right_child;
    BoundingBox BBX;
    BoundingBox BBX_world;
};

