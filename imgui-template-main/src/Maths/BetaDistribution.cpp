#include "BetaDistribution.hpp"
#include <cmath>

BetaDistribution::BetaDistribution(unsigned int seed)
    : m_rng(seed), m_uniform01(0.0, 1.0)
{}

double BetaDistribution::sampleGamma(int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; ++i)
    {
        double u = 0.0;
        do
        {
            u = m_uniform01(m_rng);
        } while (u == 0.0);
        sum += -std::log(u);
    }
    return sum;
}

float BetaDistribution::sample(int alpha, int beta)
{
    double g1 = sampleGamma(alpha);
    double g2 = sampleGamma(beta);

    return static_cast<float>(g1 / (g1 + g2));
}
