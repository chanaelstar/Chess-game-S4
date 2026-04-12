#include "PoissonDistribution.hpp"
#include <cmath>

PoissonDistribution::PoissonDistribution(unsigned int seed)
    : m_rng(seed), m_uniform01(0.0, 1.0)
{}

int PoissonDistribution::sample(double lambda)
{
    double L = std::exp(-lambda);
    int    k = 0;
    double p = 1.0;

    do
    {
        k++;
        double u = m_uniform01(m_rng);
        p *= u;
    } while (p > L);

    return k - 1;
}
