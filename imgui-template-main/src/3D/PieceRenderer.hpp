#pragma once
#include <glad/glad.h>
#include <array>
#include <glimac/Program.hpp>
#include <glm/glm.hpp>
#include <optional>
#include "3D/LightingManager.hpp"
#include "3D/ModelLoader.hpp"
#include "3D/PieceAnimator.hpp"
#include "Piece.hpp"

class ChessBoard;

class PieceRenderer {
public:
    void init();
    void draw(const ChessBoard& board, const glm::mat4& mvp, const LightingManager& lighting, const PieceAnimator& animator);

private:
    static float                   targetHeight(PieceType type);
    std::array<LoadedMesh, 7>      m_models;
    std::optional<glimac::Program> m_program;
    GLint                          m_uniMVP{}, m_uniModel{}, m_uniColor{}, m_uniTime{}, m_uniPlayerMode{};
};
