#pragma once
#include <random>

class NormalDistribution {
public:
    explicit NormalDistribution(unsigned int seed = std::random_device{}());

    float sample(float mu, float sigma);

    void samplePair(float mu, float sigma, float& z1, float& z2);

private:
    std::mt19937                           m_rng;
    std::uniform_real_distribution<double> m_uniform01;
};
