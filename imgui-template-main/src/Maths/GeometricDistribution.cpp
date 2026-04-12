#include "GeometricDistribution.hpp"
#include <cmath>

GeometricDistribution::GeometricDistribution(unsigned int seed)
    : m_rng(seed), m_uniform01(0.0, 1.0)
{}

int GeometricDistribution::sample(double p)
{
    double u = 0.0;
    do
    {
        u = m_uniform01(m_rng);
    } while (u == 0.0);
    return static_cast<int>(std::ceil(std::log(u) / std::log(1.0 - p)));
}
