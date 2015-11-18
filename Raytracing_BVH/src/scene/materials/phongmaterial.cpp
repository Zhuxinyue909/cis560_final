#include <scene/materials/phongmaterial.h>

PhongMaterial::PhongMaterial():
    PhongMaterial(glm::vec3(0.5f, 0.5f, 0.5f))
{}

PhongMaterial::PhongMaterial(const glm::vec3 &color):
    Material(color),
    specular_power(10)
{}

glm::vec3 PhongMaterial::EvaluateReflectedEnergy(const Intersection &isx, const glm::vec3 &outgoing_ray, const glm::vec3 &incoming_ray)
{
    glm::vec3 color;
    const float k_a(0.1f);
    const float k_d(0.5f);
    const float k_s(0.4f);
    //lambert part
    const glm::vec3 I_a(0.05f, 0.05f, 0.05f); //fudge factor
    glm::vec3 I_d(isx.rgb * glm::dot(isx.normal, outgoing_ray) * glm::dot(isx.normal, -incoming_ray));
    //specular part
    glm::vec3 H;
    glm::vec3 I_s;
    float S_const;

    H = (outgoing_ray - incoming_ray)/2.0f;
    S_const = pow(glm::dot(H, isx.normal), specular_power);
    if(S_const < 0) S_const = 0;
    I_s = S_const*isx.rgb;

    color = k_a*I_a + k_d*I_d + k_s*I_s;
    //modify color with reflectivity
    color = (1-reflectivity)*color;

    return color;
}
