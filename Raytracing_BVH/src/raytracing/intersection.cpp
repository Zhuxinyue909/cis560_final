#include <raytracing/intersection.h>

Intersection::Intersection():
    point(glm::vec3(0)),
    normal(glm::vec3(0)),
    rgb(glm::vec3(0)),
    t(-1)
{
    object_hit = NULL;
}

IntersectionEngine::IntersectionEngine()
{
    scene = NULL;
}

Intersection IntersectionEngine::GetIntersection(Ray r)
{
#define USEBVH

#ifndef USEBVH

    Intersection XX_obj;
    Intersection XX_scene;
    int obj_counts(scene->objects.count());
    float t_max(std::numeric_limits<float>::infinity());


    for (int i = 0; i < obj_counts; i++)
    {
        XX_obj = scene->objects[i]->GetIntersection(r);
        if ((XX_obj.object_hit != NULL) && (XX_obj.t < t_max))
        {
            XX_scene = XX_obj;
            t_max = XX_obj.t;
        }
    }

    return XX_scene;

#else

    Intersection XX_obj;

    XX_obj = root->GetIntersection(r);

    return XX_obj;

#endif

}
