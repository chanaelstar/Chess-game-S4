#pragma once
#include <random>

class BetaDistribution {
public:
    explicit BetaDistribution(unsigned int seed = std::random_device{}());
    float sample(int alpha, int beta);

private:
    std::mt19937                           m_rng;
    std::uniform_real_distribution<double> m_uniform01;
    double                                 sampleGamma(int n);
};
