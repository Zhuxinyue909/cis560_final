#include <scene/materials/lambertmaterial.h>

LambertMaterial::LambertMaterial():Material()
{}

LambertMaterial::LambertMaterial(const glm::vec3 &color):Material(color)
{}

glm::vec3 LambertMaterial::EvaluateReflectedEnergy(const Intersection &isx, const glm::vec3 &outgoing_ray, const glm::vec3 &incoming_ray)
{
    const float k_a(0.4f);
    const float k_d(0.6f);
    const glm::vec3 I_a(0.05f, 0.05f, 0.05f); //fudge factor
    glm::vec3 I_d(isx.rgb * glm::dot(isx.normal, outgoing_ray) * glm::dot(isx.normal, -incoming_ray));
    return (1-reflectivity)*glm::vec3(k_a*I_a + k_d*I_d);
}
