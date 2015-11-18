#include <scene/materials/bxdfs/bxdf.h>


glm::vec3 BxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    //TODO
    //everything here is in normal local space
    glm::vec3 sampled_color;

    wi_ret.z = rand1;
    wi_ret.x = glm::cos(TWO_PI*rand2)*glm::sqrt(1 - rand1*rand1);
    wi_ret.y = glm::sin(TWO_PI*rand2)*glm::sqrt(1 - rand1*rand1);

    //wi_ret = glm::normalize(wi_ret);
    pdf_ret = PDF(wo, wi_ret);

    //divide by the pdf_ret for importance sampling
    sampled_color = EvaluateScatteredEnergy(wo, wi_ret);


    return sampled_color;
}

glm::vec3 BxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2* samples) const
{
    //TODO
    //everything here is in normal local space
    glm::vec3 wi_temp(0, 0, 0);
    glm::vec3 temp_color(0);
    glm::vec3 final_color(0, 0, 0);
    float rand1(0.0f);
    float rand2(0.0f);
    float pdf_temp(0.0f);

    for (int i = 0; i < num_samples; i++)
    {
        rand1 = (samples + i)->x;
        rand2 = (samples + i)->y;

        temp_color = SampleAndEvaluateScatteredEnergy(wo, wi_temp, rand1, rand2, pdf_temp);
        temp_color = temp_color/pdf_temp; //importance sampling
        final_color = final_color + temp_color;
    }
    // don't forget to divide by N
    final_color = final_color/static_cast<float>(num_samples);
    return final_color;
}

float BxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    //this is assuming uniform distribution (lambert material)
    //everything here is in normal local space
    glm::vec3 default_normal(0, 0, 1);
    //wi should be normalized coming in, if not things go to shit
    float cos_theta_i(glm::dot(wi, default_normal));
    //float sin_theta_i(glm::length(glm::cross(wi, default_normal)));
    //float sin_theta_o(glm::length(glm::cross(wo, default_normal)));
    //find the pdf of the ray coming in and ray coming out
    float pdf_i(1/PI*cos_theta_i);
    //float pdf_o(1/PI*cos_theta_o*sin_theta_o);
    //in this case coming in and going out is independent
    //float pdf_result(pdf_i*pdf_o);
    //return pdf_result;
    return pdf_i;
}
