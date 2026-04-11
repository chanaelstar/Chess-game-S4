#include "UniformDiscrete.hpp"
#include <cmath>

UniformDiscrete::UniformDiscrete(unsigned int seed)
    : m_rng(seed), m_uniform01(0.0, 1.0)
{}

int UniformDiscrete::sample(int a, int b)
{
    double u = m_uniform01(m_rng);

    int n = b - a + 1;
    return a + static_cast<int>(std::floor(u * n));
}
