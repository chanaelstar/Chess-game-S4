#pragma once
#include <random>

class GeometricDistribution {
public:
    explicit GeometricDistribution(unsigned int seed = std::random_device{}());
    int sample(double p);

private:
    std::mt19937                           m_rng;
    std::uniform_real_distribution<double> m_uniform01;
};
