#include "BinomialDistribution.hpp"
#include <vector>

BinomialDistribution::BinomialDistribution(unsigned int seed)
    : m_rng(seed), m_uniform01(0.0, 1.0)
{}

int BinomialDistribution::sample(int n, double p, std::vector<bool>& trials)
{
    trials.resize(n);
    int successes = 0;
    for (int i = 0; i < n; ++i)
    {
        double u  = m_uniform01(m_rng);
        trials[i] = (u < p);
        if (trials[i])
            ++successes;
    }
    return successes;
}

int BinomialDistribution::sample(int n, double p)
{
    std::vector<bool> trials;
    return sample(n, p, trials);
}
