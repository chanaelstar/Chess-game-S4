#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <utility>
#include <vector>
#include "3D/BoardHighlight.hpp"
#include "3D/BoardRenderer.hpp"
#include "3D/CameraController.hpp"
#include "3D/LightingManager.hpp"
#include "3D/PieceAnimator.hpp"
#include "3D/PieceRenderer.hpp"
#include "3D/RayCaster.hpp"
#include "3D/SkyboxRenderer.hpp"
#include "Piece.hpp"

class ChessBoard;

class Renderer3D {
public:
    Renderer3D() = default;

    void   init();
    void   draw(const ChessBoard& board, float dt);
    GLuint getTextureId() const { return m_fboTexture; }

    static constexpr int FBO_WIDTH  = 800;
    static constexpr int FBO_HEIGHT = 800;

    // Trackball
    void orbit(float dTheta, float dPhi)  { m_camera.orbit(dTheta, dPhi); }
    void zoom(float delta)                { m_camera.zoom(delta); }
    void pan(float dx, float dy)          { m_camera.pan(dx, dy); }
    void setDistance(float d)             { m_camera.setDistance(d); }
    // PieceView
    void lookAround(float dTheta, float dPhi) { m_camera.lookAround(dTheta, dPhi); }
    // row/col : case de la pièce ; pieceTop : position monde du sommet de la pièce
    void enterPieceView(int row, int col, glm::vec3 pieceTop)
    {
        m_pvRow          = row;
        m_pvCol          = col;
        m_pvHeightOffset = pieceTop.y - 0.45f; // hauteur au-dessus du plateau
        m_camera.setPiecePosition(pieceTop);
        m_camera.setMode(CameraMode::PieceView);
    }
    // Mise à jour de la cible après un coup (la pièce est maintenant à newRow/newCol)
    void updatePieceViewTarget(int newRow, int newCol)
    {
        m_pvRow = newRow;
        m_pvCol = newCol;
    }
    void enterTrackball()    { m_camera.setMode(CameraMode::Trackball); m_pvRow = m_pvCol = -1; }
    bool isPieceView() const { return m_camera.getMode() == CameraMode::PieceView; }
    // Éclairage / couleurs
    void setChaosColors(glm::vec3 light, glm::vec3 dark) { m_colorLight = light; m_colorDark = dark; }
    void setCurrentPlayer(PieceColor p) { m_lighting.setCurrentPlayer(p); }
    // Animation (§3.7)
    void startPieceAnimation(int fromRow, int fromCol, int toRow, int toCol)
    {
        m_animator.startMove(fromRow, fromCol, toRow, toCol);
    }
    bool isAnimating()    const { return m_animator.isAnimating(); }
    bool isReadyForNext() const { return m_animator.isReadyForNext(); }
    // Sélection / hover (§3.6)
    std::pair<int,int> pickSquare(float mouseRelX, float mouseRelY, float imgW, float imgH) const;
    void setHoverSquare(int row, int col) { m_hoverRow = row; m_hoverCol = col; }
    void setSelectionDisplay(int row, int col, std::vector<std::pair<int,int>> moves)
    {
        m_selRow = row; m_selCol = col; m_validMoves = std::move(moves);
    }

private:
    // FBO
    GLuint    m_fbo{}, m_fboTexture{}, m_fboDepth{};
    glm::mat4 m_projMatrix{};
    // Sous-systèmes
    CameraController m_camera{};
    LightingManager  m_lighting{};
    PieceAnimator    m_animator{};
    BoardHighlight   m_highlight{};
    SkyboxRenderer   m_skybox{};
    BoardRenderer    m_boardRenderer{};
    PieceRenderer    m_pieceRenderer{};
    // Couleurs des cases
    glm::vec3 m_colorLight{1.f, 0.871f, 0.455f};
    glm::vec3 m_colorDark {0.804f, 0.510f, 0.247f};
    // Pièce active en Vue Pièce (bonus §5 : caméra suit la pièce)
    int   m_pvRow{-1}, m_pvCol{-1};
    float m_pvHeightOffset{0.f}; // hauteur du sommet de la pièce au-dessus de y=0.45
    // État hover / sélection (§3.6)
    int                             m_hoverRow{-1}, m_hoverCol{-1};
    int                             m_selRow{-1},   m_selCol{-1};
    std::vector<std::pair<int,int>> m_validMoves;
};
