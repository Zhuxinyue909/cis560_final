#include <scene/materials/bxdfs/blinnmicrofacetbxdf.h>

float Fresnel(glm::vec3 wo, glm::vec3 wi)
{
    //calculates the fresnel term
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    float cos_i = glm::dot(wi, N);
    float cos_o = glm::dot(wo, N);
    //index of refraction for air(incoming)
    float ni = 1.0f;
    //index of refraction for reflective material(just fudge some large number)
    float nt = 10000000.0f;
    //
    float rl = (nt*cos_i - ni*cos_o) / (nt*cos_i + ni*cos_o);
    float rp = (ni*cos_i - nt*cos_o) / (ni*cos_i + nt*cos_o);

    float F = (rl*rl + rp*rp)/2.0f;
    return F;
}

glm::vec3 BlinnMicrofacetBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    //find half vector
    glm::vec3 H(glm::normalize(wi+wo));
    //define local normal
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    //find G
    float G1 = 1.0f;
    float G2 = 2.0f * glm::dot(N, H) * glm::dot(N, wo) / glm::dot(wo, H);
    float G3 = 2.0f * glm::dot(N, H) * glm::dot(N, wi) / glm::dot(wi, H);
    float G = glm::min(G3, glm::min(G1, G2));
    //use blinn phong distribution
    float D = (exponent+2.0f)/2.0f/PI*glm::pow(glm::dot(N, H), exponent);
    //calculate fresnel term
    float F = Fresnel(wo, wi);


    glm::vec3 result(reflection_color*D*G*F/4.0f/glm::dot(N, wi)/glm::dot(N, wo));
    return result;
}
glm::vec3 BlinnMicrofacetBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    //only sample the brdf lobe
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    glm::vec3 energy(0);
    glm::vec3 lobe_vec(0);
    glm::vec3 H_sample;

    //this is samping the lobe in lobe space
    float cos_theta = glm::pow(rand1, 1.0f/(exponent+1.0f));
    float sin_theta = glm::sqrt(1.0f - cos_theta*cos_theta);
    float phi = 2.0f*PI*rand2;
    //lobe is reflected about the normal
    lobe_vec.x = -wo.x;
    lobe_vec.y = -wo.y;
    lobe_vec.z = wo.z;
    //the incident ray need to be converted back into tangent space
    H_sample.x = glm::cos(phi)*sin_theta;
    H_sample.y = glm::sin(phi)*sin_theta;
    H_sample.z = cos_theta;
    //check for half vectors that goes into the page
    if(glm::dot(H_sample, N) <= 0.0f) H_sample = -H_sample;
    //get wi given wo and H_sample

    wi_ret = -wo + 2.0f * glm::dot(wo, H_sample) * H_sample;


    pdf_ret = PDF(wo, wi_ret);

    energy = EvaluateScatteredEnergy(wo, wi_ret);

    return energy;
}

glm::vec3 BlinnMicrofacetBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}

float BlinnMicrofacetBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    glm::vec3 H(glm::normalize(wi+wo));
    glm::vec3 N(0.0f, 0.0f, 1.0f);
    float costheta = glm::dot(H, N);

    if(glm::dot(wo, H) <= 0.0f) return 0.0f;

    float blinnpdf = (exponent + 1.0f) * glm::pow(costheta, exponent) / (2.0f*PI*4.0f*glm::dot(wo, H));

    return blinnpdf;
}
