#include "imwidepdsampler.h"
#include <chrono>

ImWidePDsampler::ImWidePDsampler(){ImWidePDsampler(1);}

ImWidePDsampler::ImWidePDsampler(int samples)
{
    super_samples = samples;
}

QList<glm::vec2> ImWidePDsampler::GetSamples(int width, int height)
{
    int total_samples = super_samples*super_samples*width*height;
    float min_distance = (1.0f/static_cast<float>(super_samples)*glm::sqrt(2.0f));
    float candidate_distance(0);
    int samples_made = 0;
    float x_candidate(0);
    float y_candidate(0);
    glm::vec2 vec_candidate;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);

    QList<glm::vec2> sample_list;

    while(samples_made < total_samples)
    {
        //generate candidate

        x_candidate = static_cast<float>(width) * unif_distribution(generator);
        y_candidate = static_cast<float>(height) * unif_distribution(generator);
        vec_candidate = glm::vec2(x_candidate, y_candidate);
        bool candidate_ok = true;

        for(int i = 0; i < sample_list.count(); i++)
        {
            candidate_distance = glm::distance(sample_list[i], vec_candidate);

            if (candidate_distance > min_distance)
            {
                //proceed until end or one of the point gets rejected
            }
            else
            {
                candidate_ok = false;
                break; //to save time
            }
        }

        if(candidate_ok)
        {
            sample_list.append(glm::vec2(x_candidate, y_candidate));
            samples_made++;
        }
    }

    return sample_list;
}
