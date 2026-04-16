#pragma once
#include <random>

class CauchyDistribution {
public:
    explicit CauchyDistribution(unsigned int seed = std::random_device{}());

    float sample(float x0, float gamma);

private:
    std::mt19937                           m_rng;
    std::uniform_real_distribution<double> m_uniform01;
};
