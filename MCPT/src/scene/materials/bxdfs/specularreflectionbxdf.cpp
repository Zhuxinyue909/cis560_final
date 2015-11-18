#include <scene/materials/bxdfs/specularreflectionbxdf.h>

glm::vec3 SpecularReflectionBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    //define normal vector
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    float residule = glm::length(glm::normalize(wo+wi) - N);
    //calculate constant k to weigh the reflected energy correctly

    if(residule <= 0.00001f )
    {
        //float cos_theta = glm::dot(wi, N);
        //float sin_theta = glm::length(glm::cross(wi, N));
        //float k = 1/cos_theta/sin_theta;
        //return k*reflection_color;
        return reflection_color;
    }
    else
    {
        return glm::vec3(0);
    }
}

glm::vec3 SpecularReflectionBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    //since it's perfect specular, I will only sample in the perfect reflection direction
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    //hack, since i am in local tangent space, i can find my reflection by doing this
    wi_ret.x = -wo.x;
    wi_ret.y = -wo.y;
    wi_ret.z = wo.z;
    pdf_ret = PDF(wo, wi_ret);

    return EvaluateScatteredEnergy(wo, wi_ret);
}

glm::vec3 SpecularReflectionBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}

float SpecularReflectionBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //define normal vector
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    float residule = glm::length(glm::normalize(wo+wi) - N);

    if(residule <= 0.00001)
    {
        return 1.0f;//perfectly specular
    }
    else
    {
        return 0.0f;
    }
}
