#include "3D/LightingManager.hpp"
#include <glad/glad.h>

void LightingManager::start()
{
    m_startTime = std::chrono::steady_clock::now();
}

float LightingManager::elapsed() const
{
    return std::chrono::duration<float>(
        std::chrono::steady_clock::now() - m_startTime).count();
}

int LightingManager::playerMode() const
{
    return (m_currentPlayer == PieceColor::White) ? 0 : 1;
}

void LightingManager::applyToPieceShader(GLint uniTime, GLint uniPlayerMode) const
{
    glUniform1f(uniTime,       elapsed());
    glUniform1i(uniPlayerMode, playerMode());
}

void LightingManager::applyToBoardShader(GLint uniLightMode) const
{
    glUniform1i(uniLightMode, playerMode());
}
