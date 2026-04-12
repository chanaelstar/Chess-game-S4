#pragma once
#include <random>

class ExponentialDistribution {
public:
    explicit ExponentialDistribution(unsigned int seed = std::random_device{}());

    // Tire X ~ Exp(lambda) : temps en secondes avant le prochain événement
    float sample(float lambda);

private:
    std::mt19937                           m_rng;
    std::uniform_real_distribution<double> m_uniform01;
};
