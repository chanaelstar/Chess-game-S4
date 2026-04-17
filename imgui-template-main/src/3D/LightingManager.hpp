#pragma once
#include <glad/glad.h>
#include <chrono>
#include "Piece.hpp"

// Gère les deux modes d'éclairage (tour des blancs / tour des noirs)
class LightingManager {
public:
    LightingManager() = default;

    void start();

    void setCurrentPlayer(PieceColor p) { m_currentPlayer = p; }

    // Uniforms pour le shader des pièces (uTime + uPlayerMode)
    void applyToPieceShader(GLint uniTime, GLint uniPlayerMode) const;

    // Uniform pour le shader du plateau (uLightMode)
    void applyToBoardShader(GLint uniLightMode) const;

private:
    float elapsed() const;
    int   playerMode() const;

    PieceColor                            m_currentPlayer{PieceColor::White};
    std::chrono::steady_clock::time_point m_startTime{};
};
