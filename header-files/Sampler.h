#ifndef __SAMPLER__
#define __SAMPLER__

#include "Points.h"
#include <vector>
#include <algorithm>
#include <random>

class Sampler 
{
    public:
        virtual void generate_samples(void) = 0;
        void setup_shuffled_indices(void);
        void map_samples_to_unit_disk(void);
        int get_num_samples(void);
        void shuffle_samples(void);
        Point2D sample_unit_square(void);
        Point2D sample_unit_disk(void);

    protected:
        int n_samples = 1;
        int n_sets = 1;
        std::vector<Point2D> samples;
        std::vector<Point2D> disk_samples;
        std::vector<int> shuffled_indices;
        unsigned long count = 0;
        int jump;
};

Point2D Sampler::sample_unit_square(void)
{
    if (count % n_samples == 0)
    {
        jump = ((int)rand() % n_sets) * n_samples;
    }
    return (samples[jump + shuffled_indices[(jump + count++) % n_samples]]);
}

Point2D Sampler::sample_unit_disk(void)
{
    if (count % n_samples == 0)
    {
        jump = ((int)rand() % n_sets) * n_samples;
    }
    return (disk_samples[jump + shuffled_indices[(jump + count++) % n_samples]]);
}

int Sampler::get_num_samples()
{
    return n_samples;
}

void Sampler::setup_shuffled_indices(void)
{
    shuffled_indices.reserve(n_samples * n_sets);
    std::vector<int> indices;

    for (int j = 0; j < n_samples; j++)
    {
        indices.push_back(j);
    }

    for (int i = 0; i < n_sets; i++)
    {
        std::shuffle(indices.begin(), indices.end(), std::default_random_engine(0));
        for (int j = 0; j < n_samples; j++)
        {
            shuffled_indices.push_back(indices[j]);
        }
    }
}

void Sampler::map_samples_to_unit_disk(void)
{
    int size = samples.size();
    Point2D auxPoint = Point2D();
    float hipotenusa, x, y, theta, pDistance;

    disk_samples.reserve(size);

    for (int i = 0; i < size; i++)
    {
        auxPoint.x = 2*samples[i].x - 1;
        auxPoint.y = (-2)*samples[i].y + 1;
        x = auxPoint.x;
        y = auxPoint.y;
        pDistance = sqrtf(x*x + y*y);
        
        if (pDistance > 1.0) { 
            // put the conditions after this block, inside this block
            // to normalize only the points outsite the disk
        }

        if (x > std::abs(y)) //first disk quadrant
        {
            hipotenusa = x;
        } else if (y > std::abs(x)) { // second disk quadrant
            hipotenusa = y;
        } else if (-x > std::abs(y)) { // third disk quadrant
            hipotenusa = -x;
        } else { // fourth disk quadrant
            hipotenusa = -y;
        }

        if (x != 0 && y != 0) {
            auxPoint.y = (y*hipotenusa)/pDistance;
            auxPoint.x = (x*auxPoint.y)/y; 
        }

        disk_samples[i].x = (auxPoint.x + 1.0)/2.0;
        disk_samples[i].y = (-auxPoint.y + 1.0)/2.0;
    }
}

#endif