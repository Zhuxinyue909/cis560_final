#include <raytracing/samplers/uniformpixelsampler.h>

UniformPixelSampler::UniformPixelSampler():PixelSampler()
{}

UniformPixelSampler::UniformPixelSampler(int samples):PixelSampler(samples)
{}

QList<glm::vec2> UniformPixelSampler::GetSamples(int x, int y)
{
    float x_ul = static_cast<float>(x);
    float y_ul = static_cast<float>(y);
    float x_temp;
    float y_temp;
    float sample_dist(1.0f/static_cast<float>(samples_sqrt));
    float to_center(sample_dist/2.0f);
    QList<glm::vec2> result;

    for (int i = 0; i < samples_sqrt; i++)
    {
        for (int j = 0; j < samples_sqrt; j++)
        {
            x_temp = x_ul + static_cast<float>(i) * sample_dist + to_center;
            y_temp = y_ul + static_cast<float>(j) * sample_dist + to_center;
            result.append(glm::vec2(x_temp, y_temp));
        }
    }

    return result;
}
