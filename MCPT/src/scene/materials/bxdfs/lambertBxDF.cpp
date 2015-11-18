#include <scene/materials/bxdfs/lambertBxDF.h>

glm::vec3 LambertBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO

    //direction does not matter for lambertian material
    glm::vec3 result_color(diffuse_color/PI);

    return result_color;
}
