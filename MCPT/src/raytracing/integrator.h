#pragma once
#include <la.h>
#include <raytracing/ray.h>
#include <raytracing/intersection.h>
#include <raytracing/intersectionengine.h>
#include <scene/scene.h>
#include <random>

class Scene;

//The Integrator class recursively evaluates the path a ray takes throughout a scene
//and computes the color a ray becomes as it bounces.
//It samples the materials, probability density functions, and BRDFs of the surfaces the ray hits
//to do this.

class Integrator
{
public:
    Integrator();
    Integrator(Scene *s);
    virtual glm::vec3 TraceRay(Ray r, unsigned int depth);
    void SetDepth(unsigned int depth);
    //Samples taken must be set in the function. It tells the how many ray samples were used to estimate the direct
    glm::vec3 EstimateDirectLighting(const Intersection &isx, unsigned int &samples_taken, const glm::vec3 &woW);
    glm::vec3 CalculateEnergy(const Intersection &light_sample_isx, const Intersection &isx, const Ray &light_sample, const glm::vec3 &woW);

    Scene* scene;
    IntersectionEngine* intersection_engine;

protected:
    unsigned int max_depth;//Default value is 5.
    std::mt19937 mersenne_generator;
    std::uniform_real_distribution<float> unif_distribution;
};

class DirectLightingIntegrator: public Integrator
{
public:
    DirectLightingIntegrator(): Integrator() {}
    virtual glm::vec3 TraceRay(Ray r, unsigned int depth);
    glm::vec3 EstimateDirectLighting(const Intersection &isx, unsigned int &n_f, unsigned int &n_g, const glm::vec3 &woW);
    glm::vec3 LightPDFEnergy(const Intersection &light_sample_isx, const Intersection &isx, const Ray &light_sample, const glm::vec3 &woW, unsigned int n_light, unsigned int n_brdf);
    glm::vec3 BxDFPDFEnergy(const Intersection &isx, const glm::vec3 &woW, unsigned int n_light, unsigned int n_brdf);
    float MIS(float f_PDF, float g_PDF);

};

class AllLightingIntegrator: public DirectLightingIntegrator
{
public:
    AllLightingIntegrator(): DirectLightingIntegrator() {}
    virtual glm::vec3 TraceRay(Ray r, unsigned int depth);
    glm::vec3 EstimateIndirectLighting(const Intersection &isx, const unsigned int &n_split, const glm::vec3 &woW);
    glm::vec3 BxDFIndirectEnergy(const Intersection &isx, unsigned int n_split, const glm::vec3 &woW);
    glm::vec3 LightIndirectEnergy(const Intersection &isx, unsigned int n_split, const glm::vec3 &woW);

};
