#pragma once

#include<scene/materials/bxdfs/bxdf.h>

class Blinn_PhongBxdf: public BxDF
{
public:
    Blinn_PhongBxdf(const glm::vec3 &color, const float &reflectance) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), specular_color(color), specular_reflectance(reflectance)
    {specular_power = 8.0f;}

    virtual glm::vec3 EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const;

    glm::vec3 specular_color;
    float specular_reflectance;
    float specular_power;
};
