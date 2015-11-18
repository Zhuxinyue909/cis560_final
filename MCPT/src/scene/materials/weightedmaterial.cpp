#include <scene/materials/weightedmaterial.h>

WeightedMaterial::WeightedMaterial() : Material()
{
}
WeightedMaterial::WeightedMaterial(const glm::vec3 &color) : Material(color){}

glm::vec3 WeightedMaterial::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, float pdf_ret, BxDFType flags) const
{
    //TODO
    //generate some random number
    int res = 100000000;
    float rand1 = static_cast<float>(rand()%res) / static_cast<float>(res);
    float CPF = 0.0f;
    int i = 0;
    int bxdf_choice = 0;

    while(rand1 > CPF)
    {
        CPF = CPF + bxdf_weights[i];
        bxdf_choice = i;
        i++;
    }

    glm::vec3 energy;
    //use tangent and bitangent at the intersection to transform the light rays into local frame for brdf
    //find the rotation matrix from local tangents
    glm::mat3 R_inv(isx.tangent, isx.bitangent, isx.normal);
    glm::mat3 R (glm::transpose(R_inv));
    glm::vec3 wo_local(glm::normalize(R*woW));
    glm::vec3 wi_local(glm::normalize(R*wiW));

    pdf_ret = bxdfs[bxdf_choice]->PDF(wo_local, wi_local);

    energy = bxdfs[bxdf_choice]->EvaluateScatteredEnergy(wo_local, wi_local);

    return energy;
}

glm::vec3 WeightedMaterial::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, float rand1, float rand2, BxDFType flags) const
{
    int res = 100000000;
    float rand3 = static_cast<float>(rand()%res) / static_cast<float>(res);
    float CPF = 0.0f;
    int i = 0;
    int bxdf_choice = 0;

    while(rand3 > CPF)
    {
        CPF = CPF + bxdf_weights[i];
        bxdf_choice = i;
        i++;
    }

    glm::vec3 energy;
    //use tangent and bitangent at the intersection to transform the light rays into local frame for brdf
    //find the rotation matrix from local tangents
    glm::mat3 R_inv(isx.tangent, isx.bitangent, isx.normal);
    glm::mat3 R (glm::transpose(R_inv));
    glm::vec3 wo_local(glm::normalize(R*woW));
    glm::vec3 wi_local(0);

    energy = bxdfs[bxdf_choice]->SampleAndEvaluateScatteredEnergy(wo_local, wi_local, rand1, rand2, pdf_ret);

    wiW_ret = R_inv*wi_local;


    return energy;
}
