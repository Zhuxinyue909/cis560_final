#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <iostream>
#include <functional>
#include <chrono>

StratifiedPixelSampler::StratifiedPixelSampler():PixelSampler(){}

StratifiedPixelSampler::StratifiedPixelSampler(int samples) : PixelSampler(samples), mersenne_generator(), unif_distribution(0,1){}

QList<glm::vec2> StratifiedPixelSampler::GetSamples(int x, int y)
{
    float x_ul = static_cast<float>(x);
    float y_ul = static_cast<float>(y);
    float samples = static_cast<float>(samples_sqrt);
    float x_temp;
    float y_temp;

    QList<glm::vec2> result;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);

    for(int i = 0; i < samples_sqrt; i++)
    {
        for(int j = 0; j < samples_sqrt; j++)
        {
            //fix these later
            x_temp = x_ul + static_cast<float>(i)/samples + unif_distribution(generator)/samples;
            y_temp = y_ul + static_cast<float>(j)/samples + unif_distribution(generator)/samples;
            result.append(glm::vec2(x_temp, y_temp));
        }
    }
    return result;
}
