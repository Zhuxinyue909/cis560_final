#include <raytracing/integrator.h>


Integrator::Integrator():
    max_depth(5)
{
    scene = NULL;
    intersection_engine = NULL;
}

//Basic ray trace
glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth)
{

    if(depth >= max_depth) return glm::vec3(0,0,0);//first check the depth of the reflection

    Intersection objX; //stores the intersection with the objects in the scene
    glm::vec3 color(0); //stores the color
    const float episolon(0.001); //small distance
    glm::vec3 Dir_reflected; //direction of reflected ray
    glm::vec3 Dir_refracted; //direction of refracted ray
    glm::vec3 POI_cast; //point to cast reflected ray
    glm::vec3 POI_refracted; //point to cast refracted ray
    glm::vec3 reflected_color;
    glm::vec3 shadow(1,1,1);
    int light_counts = scene->lights.count();

    //first check if the ray intersects with any objects
    objX = intersection_engine->GetIntersection(r);
    //if my intersection engine didn't hit anything, just return black
    if(objX.object_hit == NULL) return color;

    //if I hit a light source, just return the base color of the light source
    if(objX.object_hit->material->emissive) return objX.object_hit->material->base_color;


    if(!fequal(objX.object_hit->material->refract_idx_in ,0.0f) || !fequal(objX.object_hit->material->refract_idx_out, 0.0f))
    {
        float enter(objX.object_hit->material->refract_idx_out/objX.object_hit->material->refract_idx_in);
        float exit(objX.object_hit->material->refract_idx_in/objX.object_hit->material->refract_idx_out);
        shadow = objX.object_hit->material->base_color;

        if(glm::dot(objX.normal, r.direction) < 0)
        {
            //if this holds true, the ray enters the refractive material
            Dir_refracted = glm::refract(r.direction, objX.normal, enter);//calculate direction of refraction assuming entering
        }
        else
        {
            Dir_refracted = glm::refract(r.direction, -objX.normal, exit);
        }

        POI_refracted = (objX.point + episolon*r.direction);//to avoid the shadow acne equivalent, traverse the refracted direction for a small distance episolon
        Ray refracted(POI_refracted, Dir_refracted);//create refracted ray

        color = TraceRay(refracted, depth + 1);

        //apply shadow
        color.r = shadow.r * color.r;
        color.g = shadow.g * color.g;
        color.b = shadow.b * color.b;

    }
    else
    {
        //find the direction of reflection
        Dir_reflected = (r.direction - 2.0f*objX.normal*glm::dot(objX.normal, r.direction));
        //traverse the direction of reflection a small distance
        POI_cast = (objX.point + episolon*objX.normal);
        Ray reflected(POI_cast, Dir_reflected); // create reflected ray

        for(int i = 0; i < light_counts; i++)
        {
            //check if the point is in shadow


            glm::vec3 Dir_LF(scene->lights[i]->transform.position() - POI_cast);

            Ray LF (POI_cast, Dir_LF);

            if(ShadowTest(scene->lights[i], LF))
            {
                shadow = glm::vec3(0, 0, 0);
            }
            else
            {
                shadow = scene->lights[i]->material->base_color;
            }

            //find the direction of light freer ray

            //apply lighting and sum shit up
            glm::vec3 base_color(objX.object_hit->material->EvaluateReflectedEnergy(objX, -r.direction, -LF.direction));
            color.r = color.r + shadow.r*base_color.r;
            color.g = color.g + shadow.g*base_color.g;
            color.b = color.b + shadow.b*base_color.b;

        }


        if(objX.object_hit->material->reflectivity > 0)
        {
            //Cast reflected ray again with +1 depth
            reflected_color = objX.object_hit->material->reflectivity * TraceRay(reflected, depth + 1) ;
            reflected_color.r = objX.object_hit->material->base_color.r * reflected_color.r;
            reflected_color.g = objX.object_hit->material->base_color.g * reflected_color.g;
            reflected_color.b = objX.object_hit->material->base_color.b * reflected_color.b;
            color = color + reflected_color;
        }
    }
    //clamp color that is too bright
    if(color.r > 1) color.r = 1;
    if(color.g > 1) color.g = 1;
    if(color.b > 1) color.b = 1;

    return color;
}
bool Integrator::ShadowTest(Geometry* light_source, Ray light_freer)
{
    Intersection objX;
    glm::vec3 Dir_LF; // stores the direction of the light freer rays
    //check if the point of reflection can see a light source
    //find the direction from POI to light source
    Dir_LF = light_freer.direction;
    //cast a light freer ray
    objX = intersection_engine->GetIntersection(light_freer);

    if(objX.object_hit == light_source)
    {
        return false;
    }
    else if(objX.object_hit == NULL)
    {
        return false;
    }
    else if(!fequal(objX.object_hit->material->refract_idx_in, 0.0f))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}
