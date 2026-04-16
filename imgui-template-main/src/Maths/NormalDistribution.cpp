#include "NormalDistribution.hpp"
#include <cmath>

static constexpr double PI = 3.14159265358979323846;

NormalDistribution::NormalDistribution(unsigned int seed)
    : m_rng(seed), m_uniform01(0.0, 1.0)
{}

void NormalDistribution::samplePair(float mu, float sigma, float& z1, float& z2)
{
    double u1 = 0.0;
    do
    {
        u1 = m_uniform01(m_rng);
    } while (u1 == 0.0);
    double u2 = m_uniform01(m_rng);

    double r     = std::sqrt(-2.0 * std::log(u1));
    double theta = 2.0 * PI * u2;

    z1 = static_cast<float>(mu + sigma * r * std::cos(theta));
    z2 = static_cast<float>(mu + sigma * r * std::sin(theta));
}

float NormalDistribution::sample(float mu, float sigma)
{
    float z1, z2;
    samplePair(mu, sigma, z1, z2);
    return z1;
}
