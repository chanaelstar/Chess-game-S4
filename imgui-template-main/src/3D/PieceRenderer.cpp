#include "3D/PieceRenderer.hpp"
#include <glimac/FilePath.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ChessBoard.hpp"

float PieceRenderer::targetHeight(PieceType type)
{
    switch (type)
    {
    case PieceType::Pawn: return 0.40f;
    case PieceType::Rook: return 0.50f;
    case PieceType::Knight: return 0.55f;
    case PieceType::Bishop: return 0.65f;
    case PieceType::Queen: return 0.80f;
    case PieceType::King: return 0.90f;
    default: return 0.45f;
    }
}

void PieceRenderer::init()
{
    m_program = glimac::loadProgram(
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/piece.vs.glsl",
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/piece.fs.glsl"
    );
    m_program->use();
    m_uniMVP        = glGetUniformLocation(m_program->getGLId(), "uMVPMatrix");
    m_uniModel      = glGetUniformLocation(m_program->getGLId(), "uModelMatrix");
    m_uniColor      = glGetUniformLocation(m_program->getGLId(), "uColor");
    m_uniTime       = glGetUniformLocation(m_program->getGLId(), "uTime");
    m_uniPlayerMode = glGetUniformLocation(m_program->getGLId(), "uPlayerMode");

    const std::string obj                         = std::string(CMAKE_SOURCE_DIR) + "/assets/models/Chess_set_game.obj";
    m_models[static_cast<int>(PieceType::Pawn)]   = loadOBJObject(obj, "Pedone1");
    m_models[static_cast<int>(PieceType::Rook)]   = loadOBJObject(obj, "Torre1");
    m_models[static_cast<int>(PieceType::Knight)] = loadOBJObject(obj, "Cavallo1");
    m_models[static_cast<int>(PieceType::Bishop)] = loadOBJObject(obj, "Alfiere1");
    m_models[static_cast<int>(PieceType::Queen)]  = loadOBJObject(obj, "Regina");
    m_models[static_cast<int>(PieceType::King)]   = loadOBJObject(obj, "Re");
}

void PieceRenderer::draw(const ChessBoard& board, const glm::mat4& mvp, const LightingManager& lighting, const PieceAnimator& animator)
{
    if (!m_program)
        return;
    m_program->use();
    lighting.applyToPieceShader(m_uniTime, m_uniPlayerMode);

    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
        {
            const Piece* piece = board.getPiece(row, col);
            if (!piece)
                continue;

            const LoadedMesh& mesh = m_models[static_cast<int>(piece->getType())];
            if (!mesh.valid)
                continue;

            // Position animée ou statique
            glm::vec3 piecePos(col - 4.f + 0.5f, 0.45f, row - 4.f + 0.5f);
            animator.getAnimatedPos(row, col, piecePos);

            // Rotation : direction de déplacement pendant l'animation,
            // orientation par défaut (cavalier blanc = 180°)
            float rotY     = (piece->getType() == PieceType::Knight
                          && piece->getColor() == PieceColor::White)
                                 ? glm::radians(180.f)
                                 : 0.f;
            float animRotY = 0.f;
            if (animator.getAnimatedRotY(row, col, animRotY))
                rotY = animRotY; // face la direction de déplacement

            glm::vec3 bboxCenter = (mesh.bboxMin + mesh.bboxMax) * 0.5f;
            float     modelH     = mesh.bboxMax.y - mesh.bboxMin.y;
            float     scale      = (modelH > 0.f) ? targetHeight(piece->getType()) / modelH : 1.f;

            glm::mat4 model = glm::translate(glm::mat4(1.f), piecePos)
                              * glm::scale(glm::mat4(1.f), glm::vec3(scale))
                              * glm::rotate(glm::mat4(1.f), rotY, glm::vec3(0.f, 1.f, 0.f))
                              * glm::translate(glm::mat4(1.f), glm::vec3(-bboxCenter.x, -mesh.bboxMin.y, -bboxCenter.z));

            glm::vec3 color = (piece->getColor() == PieceColor::White)
                                  ? glm::vec3(0.95f, 0.93f, 0.85f)
                                  : glm::vec3(0.12f, 0.08f, 0.05f);

            glUniformMatrix4fv(m_uniMVP, 1, GL_FALSE, glm::value_ptr(mvp * model));
            glUniformMatrix4fv(m_uniModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(m_uniColor, 1, glm::value_ptr(color));

            glBindVertexArray(mesh.vao);
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
        }
}
