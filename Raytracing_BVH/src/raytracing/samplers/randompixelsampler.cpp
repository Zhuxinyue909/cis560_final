#include "randompixelsampler.h"
#include <chrono>

RandomPixelSampler::RandomPixelSampler():PixelSampler(){}

RandomPixelSampler::RandomPixelSampler(int samples) : PixelSampler(samples), mersenne_generator(), unif_distribution(0,1){}

QList<glm::vec2> RandomPixelSampler::GetSamples(int x, int y)
{
    float x_ul = static_cast<float>(x);
    float y_ul = static_cast<float>(y);
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
            x_temp = x_ul + unif_distribution(generator);
            y_temp = y_ul + unif_distribution(generator);
            result.append(glm::vec2(x_temp, y_temp));
        }
    }
    return result;
}
