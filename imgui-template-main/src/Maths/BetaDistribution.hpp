#pragma once
#include <random>

class BetaDistribution {
public:
    explicit BetaDistribution(unsigned int seed = std::random_device{}());

    // Tire X ~ Beta(alpha, beta) avec alpha, beta entiers >= 1
    float sample(int alpha, int beta);

private:
    std::mt19937                           m_rng;
    std::uniform_real_distribution<double> m_uniform01;

    // Simule Gamma(n, 1) = -sum_{i=1}^{n} ln(U_i)
    double sampleGamma(int n);
};
