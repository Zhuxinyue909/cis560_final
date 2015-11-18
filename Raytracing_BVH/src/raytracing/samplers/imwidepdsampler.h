#pragma once
#include<la.h>
#include<random>
#include<QList>

class ImWidePDsampler
{
public:
    ImWidePDsampler();
    ImWidePDsampler(int samples);
    QList<glm::vec2> GetSamples(int width, int height); //since this class is image-wide,
                                               //this function then takes the width and
                                               //height of the image

protected:

    std::mt19937 mersenne_generator;
    std::uniform_real_distribution<float> unif_distribution;
    int super_samples;
};
