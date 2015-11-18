#include "blinn_phongbxdf.h"

glm::vec3 Blinn_PhongBxdf::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    glm::vec3 H ((wo+wi)/2.0f);
    glm::vec3 N(0.0f, 0.0f, 1.0f);

    float Cos_delta(glm::abs(glm::dot(H, N)));
    float Energy(specular_reflectance*(specular_power+2)/TWO_PI*pow(Cos_delta, specular_power));

    return Energy*specular_color;
}

