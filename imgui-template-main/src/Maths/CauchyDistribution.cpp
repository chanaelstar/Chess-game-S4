#include "CauchyDistribution.hpp"
#include <cmath> // tan

static constexpr double PI = 3.14159265358979323846;

CauchyDistribution::CauchyDistribution(unsigned int seed)
    : m_rng(seed), m_uniform01(0.0, 1.0)
{}

float CauchyDistribution::sample(float x0, float gamma)
{
    double u = 0.0;
    do
    {
        u = m_uniform01(m_rng);
    } while (u == 0.0 || u == 1.0);
    return static_cast<float>(x0 + gamma * std::tan(PI * (u - 0.5)));
}
