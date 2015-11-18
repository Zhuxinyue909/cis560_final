#include "bvhnode.h"
#include "scene/geometry/mesh.h"

BVHNode::BVHNode()
{
    BBX = BoundingBox();
    left_child = NULL;
    right_child = NULL;
    obj_enclosed = NULL;
}

//less than functions for qsort
bool XLessThan(const Geometry* G1, const Geometry* G2)
{
    return (G1->BBX->GetCenter().x) < (G2->BBX->GetCenter().x);
}

bool YLessThan(const Geometry* G1, const Geometry* G2)
{
    return (G1->BBX->GetCenter().y) < (G2->BBX->GetCenter().y);
}

bool ZLessThan(const Geometry* G1, const Geometry* G2)
{
    return (G1->BBX->GetCenter().z) < (G2->BBX->GetCenter().z);
}

BVHNode::BVHNode(QList<Geometry*> &objects, int axis)
{
    float episolon(0.001f);
    if(objects.count() == 1) //if i am at the end of the tree
    {
        left_child = NULL;
        right_child = NULL;
        //take the same bounding box as the object
        BBX = BoundingBox(objects[0]->BBX->max_bound(), objects[0]->BBX->min_bound());
        BBX.create();
        obj_enclosed = objects[0];
    }
    else
    {
        int next_axis;
        int median_idx;
        //first set the bounding box values
        glm::vec3 max_bounds(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
        glm::vec3 min_bounds(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
        //find the max and min of all the bounding boxes
        for(int i = 0; i < objects.count(); i++)
        {
            max_bounds.x = glm::max(objects[i]->BBX->max_bound().x, max_bounds.x);
            max_bounds.y = glm::max(objects[i]->BBX->max_bound().y, max_bounds.y);
            max_bounds.z = glm::max(objects[i]->BBX->max_bound().z, max_bounds.z);

            min_bounds.x = glm::min(objects[i]->BBX->min_bound().x, min_bounds.x);
            min_bounds.y = glm::min(objects[i]->BBX->min_bound().y, min_bounds.y);
            min_bounds.z = glm::min(objects[i]->BBX->min_bound().z, min_bounds.z);
        }
        //put the values into my bounding box
        BBX = BoundingBox(max_bounds+episolon, min_bounds-episolon);
        BBX.create();


        //partition the objects for the child nodes
        switch (axis)
        {
        case 0:
            qSort(objects.begin(), objects.end(), XLessThan);
            next_axis = 1;
            break;
        case 1:
            qSort(objects.begin(), objects.end(), YLessThan);
            next_axis = 2;
            break;
        case 2:
            qSort(objects.begin(), objects.end(), ZLessThan);
            next_axis = 0;
            break;
        }

        median_idx = objects.count()/2;

        QList<Geometry*> left_obj = objects.mid(0, median_idx);
        QList<Geometry*> right_obj = objects.mid(median_idx);

        left_child = new BVHNode(left_obj, next_axis);
        right_child = new BVHNode(right_obj, next_axis);
        obj_enclosed = NULL;
    }
}

BVHNode::BVHNode(QList<Triangle*> &objects, int axis)
{
    float episolon(0.001f);
    float split_res(0); // resolution for splitting the box
    float split_boundary(0);
    int bins(10); //number of bins for splitting the box
    if(objects.count() == 1) //if i am at the end of the tree
    {
        left_child = NULL;
        right_child = NULL;
        //take the same bounding box as the object
        BBX = BoundingBox(objects[0]->BBX->max_bound(), objects[0]->BBX->min_bound());
        //BBX.create();
        BBX_world = BoundingBox(objects[0]->BBX_world->max_bound(), objects[0]->BBX_world->min_bound());
        BBX_world.create();
        obj_enclosed = objects[0];
    }
    else
    {
        int next_axis;
        int median_idx;
        //first set the bounding box values for intersection
        glm::vec3 max_bounds(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
        glm::vec3 min_bounds(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
        //find the max and min of all the bounding boxes
        for(int i = 0; i < objects.count(); i++)
        {
            max_bounds.x = glm::max(objects[i]->BBX->max_bound().x, max_bounds.x);
            max_bounds.y = glm::max(objects[i]->BBX->max_bound().y, max_bounds.y);
            max_bounds.z = glm::max(objects[i]->BBX->max_bound().z, max_bounds.z);

            min_bounds.x = glm::min(objects[i]->BBX->min_bound().x, min_bounds.x);
            min_bounds.y = glm::min(objects[i]->BBX->min_bound().y, min_bounds.y);
            min_bounds.z = glm::min(objects[i]->BBX->min_bound().z, min_bounds.z);
        }


        //put the values into my bounding box for intersection
        BBX = BoundingBox(max_bounds+episolon, min_bounds-episolon);
        //BBX.create();

        //this bounding box for drawing
        max_bounds = glm::vec3(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
        min_bounds = glm::vec3(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
        //find the max and min of all the bounding boxes
        for(int i = 0; i < objects.count(); i++)
        {
            max_bounds.x = glm::max(objects[i]->BBX_world->max_bound().x, max_bounds.x);
            max_bounds.y = glm::max(objects[i]->BBX_world->max_bound().y, max_bounds.y);
            max_bounds.z = glm::max(objects[i]->BBX_world->max_bound().z, max_bounds.z);

            min_bounds.x = glm::min(objects[i]->BBX_world->min_bound().x, min_bounds.x);
            min_bounds.y = glm::min(objects[i]->BBX_world->min_bound().y, min_bounds.y);
            min_bounds.z = glm::min(objects[i]->BBX_world->min_bound().z, min_bounds.z);
        }

        BBX_world = BoundingBox(max_bounds, min_bounds);
        BBX_world.create();


        //partition the objects for the child nodes
        switch (axis)
        {
        case 0:
            qSort(objects.begin(), objects.end(), XLessThan);
            next_axis = 1;
            break;
        case 1:
            qSort(objects.begin(), objects.end(), YLessThan);
            next_axis = 2;
            break;
        case 2:
            qSort(objects.begin(), objects.end(), ZLessThan);
            next_axis = 0;
            break;
        }

        QList<Triangle*> left_obj;
        QList<Triangle*> right_obj;
/*
        split_res = (BBX.max_bound()[axis] - BBX.min_bound()[axis])/static_cast<float>(bins);

        //starting at the min side of the bounding box
        int split_idx(0);
        int split_idx_best(0);
        float k(0);
        float sah_old(std::numeric_limits<float>::infinity());
        float sah(10000000000.0f); //lol

        while (sah < sah_old)
        {
            sah_old = sah;
            split_idx_best = split_idx;

            k = k+1.0f;//move the splitting plane one bin over
            split_boundary = BBX.min_bound()[axis] + k*split_res;

            //find the split point for the plane
            while(split_boundary > objects[split_idx]->BBX->GetCenter()[axis])
            {
                split_idx++;

                if (split_idx == objects.count())
                {
                    int for_debug(0);
                }
            }

            if (split_idx == 0 || split_idx == objects.count()-1)
            {
                break; // hack for now to avoid empty list
            }

            //make the bounding box with the new split

            left_obj = objects.mid(0, split_idx);
            right_obj = objects.mid(split_idx);

            BoundingBox leftbox = MakeBox(left_obj);
            BoundingBox rightbox = MakeBox(right_obj);
            //evaluate new sah
            sah = (evaluate_SAH(leftbox, rightbox, left_obj.count(), right_obj.count()));
        }

        median_idx = split_idx_best;
*/
        median_idx = objects.count()/2;

        left_obj = objects.mid(0, median_idx);
        right_obj = objects.mid(median_idx);

        //for safety against passing in a 0 length qlist into the constructor
        if (left_obj.count() == 0)
        {
            left_child = NULL;
        }
        else
        {
            left_child = new BVHNode(left_obj, next_axis);
        }

        if (right_obj.count() == 0)
        {
            right_child = NULL;
        }
        else
        {
            right_child = new BVHNode(right_obj, next_axis);
        }

        obj_enclosed = NULL;
    }
}

BoundingBox BVHNode::MakeBox(QList<Triangle *> objects)
{
    glm::vec3 max_bounds(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    glm::vec3 min_bounds(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
    //find the max and min of all the bounding boxes
    for(int i = 0; i < objects.count(); i++)
    {
        max_bounds.x = glm::max(objects[i]->BBX->max_bound().x, max_bounds.x);
        max_bounds.y = glm::max(objects[i]->BBX->max_bound().y, max_bounds.y);
        max_bounds.z = glm::max(objects[i]->BBX->max_bound().z, max_bounds.z);

        min_bounds.x = glm::min(objects[i]->BBX->min_bound().x, min_bounds.x);
        min_bounds.y = glm::min(objects[i]->BBX->min_bound().y, min_bounds.y);
        min_bounds.z = glm::min(objects[i]->BBX->min_bound().z, min_bounds.z);
    }

    return BoundingBox(max_bounds, min_bounds);
}

float BVHNode::evaluate_SAH(BoundingBox BBX_left, BoundingBox BBX_right, int left_triangles, int right_triangles)
{
    float K_t(1.0f);
    float K_1(0.5f);
    float sah(0);
    sah = K_t + K_1*(surface_area(BBX_left)/surface_area(BBX)*static_cast<float>(left_triangles) + surface_area(BBX_right)/surface_area(BBX)*static_cast<float>(right_triangles));
    return sah;
}

float BVHNode::surface_area(BoundingBox Box)
{
    glm::vec3 sides(Box.max_bound() - Box.min_bound());
    float area(0);
    area = 2*sides.x*sides.y + 2*sides.x*sides.z + 2*sides.y*sides.z;
    return area;
}

Intersection BVHNode::GetIntersection(Ray r)
{
    //initialize some intersection XX to return later
    Intersection XXleft;
    Intersection XXright;

    float t0; //near intersection
    float t1; //far intersection

    //start with t_near and t_far at negative and positive infinity
    float t_near(-std::numeric_limits<float>::infinity());
    float t_far(std::numeric_limits<float>::infinity());

    //if I am inside the bounding box, I am gauranteed to intersect with the box (for reflection and refractions)
    if (r.origin.x <= BBX.max_bound().x && r.origin.x >= BBX.min_bound().x &&
            r.origin.y <= BBX.max_bound().y && r.origin.y >= BBX.min_bound().y &&
            r.origin.z <= BBX.max_bound().z && r.origin.y >= BBX.min_bound().z)
    {
        //proceed
    }
    else
        //check for intersection with the bounding box
    {
        //short cut! if I am parallel to x, just check if I am within x slab's boundary
        if (r.direction.x == 0)
        {
            if(r.origin.x < BBX.min_bound().x || r.origin.x > BBX.max_bound().x)
            {
                return XXleft;
            }
        }
        //calculate t0,t1
        t0 = (BBX.min_bound().x - r.origin.x) / r.direction.x;
        t1 = (BBX.max_bound().x - r.origin.x) / r.direction.x;
        //make appropriate swaps
        if (t0 > t1)
        {
            float temp(t0);
            t0 = t1;
            t1 = temp;
        }

        if (t0 > t_near) t_near = t0;
        if (t1 < t_far) t_far = t1;

        //check for miss
        if(t_near > t_far) return XXleft;

        //repeat for y
        if (r.direction.y == 0)
        {
            if(r.origin.y < BBX.min_bound().y || r.origin.y > BBX.max_bound().y)
            {
                return XXleft;
            }
        }
        //calculate t0,t1
        t0 = (BBX.min_bound().y - r.origin.y) / r.direction.y;
        t1 = (BBX.max_bound().y - r.origin.y) / r.direction.y;
        //make appropriate swaps
        if (t0 > t1)
        {
            float temp(t0);
            t0 = t1;
            t1 = temp;
        }

        if (t0 > t_near) t_near = t0;
        if (t1 < t_far) t_far = t1;

        //check for miss
        if(t_near > t_far) return XXleft;

        //repeat for z
        if (r.direction.z == 0)
        {
            if(r.origin.z < BBX.min_bound().z || r.origin.z > BBX.max_bound().z)
            {
                return XXleft;
            }
        }
        //calculate t0,t1
        t0 = (BBX.min_bound().z - r.origin.z) / r.direction.z;
        t1 = (BBX.max_bound().z - r.origin.z) / r.direction.z;
        //make appropriate swaps
        if (t0 > t1)
        {
            float temp(t0);
            t0 = t1;
            t1 = temp;
        }

        if (t0 > t_near) t_near = t0;
        if (t1 < t_far) t_far = t1;

        //check for miss
        if(t_near > t_far) return XXleft;
        //check for behind the ray
        if(t_near < 0) return XXleft;
    }
    //if we did not miss, check if I am leaf node, if I am a leaf node, just return the intersection with the object
    if (obj_enclosed != NULL)
    {
        return obj_enclosed->GetIntersection(r);
    }
    //check both left and right child for intersections
    XXleft = left_child->GetIntersection(r);
    XXright = right_child->GetIntersection(r);
    //if miss the left child
    if(XXleft.object_hit == NULL)
    {
        //if the right misses, no big deal
        return XXright;
    }
    else if(XXright.object_hit != NULL) //if we didn't miss either
    {
        if (XXleft.t < XXright.t) return XXleft;
        else return XXright;
    }
    else return XXleft;


}

void BVHNode::Clear()
{
    delete left_child;
    delete right_child;
}
