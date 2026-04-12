#include "ExponentialDistribution.hpp"
#include <cmath> // log

ExponentialDistribution::ExponentialDistribution(unsigned int seed)
    : m_rng(seed), m_uniform01(0.0, 1.0)
{}

float ExponentialDistribution::sample(float lambda)
{
    double u = 0.0;
    do
    {
        u = m_uniform01(m_rng);
    } while (u == 0.0);

    return static_cast<float>(-std::log(u) / lambda);
}
