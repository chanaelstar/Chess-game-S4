#pragma once
#include <random>

class UniformDiscrete {
public:
    explicit UniformDiscrete(unsigned int seed = std::random_device{}());
    int sample(int a, int b);

private:
    std::mt19937                           m_rng;
    std::uniform_real_distribution<double> m_uniform01;
};
