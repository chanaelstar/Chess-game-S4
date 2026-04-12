#pragma once
#include <random>

class BinomialDistribution {
public:
    explicit BinomialDistribution(unsigned int seed = std::random_device{}());

    int sample(int n, double p, std::vector<bool>& trials);
    int sample(int n, double p);

private:
    std::mt19937                           m_rng;
    std::uniform_real_distribution<double> m_uniform01;
};
