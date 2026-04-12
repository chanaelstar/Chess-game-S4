#pragma once
#include <random>

class PoissonDistribution {
public:
    explicit PoissonDistribution(unsigned int seed = std::random_device{}());
    int sample(double lambda);

private:
    std::mt19937                           m_rng;
    std::uniform_real_distribution<double> m_uniform01;
};
