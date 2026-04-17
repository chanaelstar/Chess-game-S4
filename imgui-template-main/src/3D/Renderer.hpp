#pragma once
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glimac/Sphere.hpp>
#include <glm/glm.hpp>
#include <array>
#include <optional>
#include "3D/BoardHighlight.hpp"
#include "3D/CameraController.hpp"
#include "3D/LightingManager.hpp"
#include "3D/ModelLoader.hpp"
#include "3D/RayCaster.hpp"
#include "Piece.hpp"

class ChessBoard;

class Renderer3D {
public:
    Renderer3D() = default;

    void   init();
    void   draw(const ChessBoard& board);
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
    void enterPieceView(glm::vec3 pieceTop)   { m_camera.setPiecePosition(pieceTop); m_camera.setMode(CameraMode::PieceView); }
    void enterTrackball()                     { m_camera.setMode(CameraMode::Trackball); }
    bool isPieceView() const                  { return m_camera.getMode() == CameraMode::PieceView; }
    // Éclairage / couleurs
    void setChaosColors(glm::vec3 light, glm::vec3 dark);
    void setCurrentPlayer(PieceColor p) { m_lighting.setCurrentPlayer(p); }
    // Sélection / hover (§3.6)
    std::pair<int,int> pickSquare(float mouseRelX, float mouseRelY, float imgW, float imgH) const;
    void setHoverSquare(int row, int col) { m_hoverRow = row; m_hoverCol = col; }
    void setSelectionDisplay(int row, int col, std::vector<std::pair<int,int>> moves)
    {
        m_selRow = row; m_selCol = col; m_validMoves = std::move(moves);
    }

private:
    void buildBoardMesh();
    void buildBorderMesh();
    void buildPieceMesh();
    void buildSkyboxMesh();
    GLuint                         m_vao{};
    GLuint                         m_vbo{};
    GLuint                         m_borderVao{};
    GLuint                         m_borderVbo{};
    int                            m_borderVertexCount{};
    GLuint                         m_pieceVao{};
    GLuint                         m_pieceVbo{};
    int                            m_pieceVertexCount{};
    GLuint                         m_skyboxVao{};
    GLuint                         m_skyboxVbo{};
    std::optional<glimac::Program> m_skyboxProgram;
    GLint                          m_skyboxUniVP{};
    std::optional<glimac::Program> m_program;
    // Shader + modèles 3D des pièces
    std::optional<glimac::Program> m_pieceProgram;
    GLint                          m_pieceUniMVP{};
    GLint                          m_pieceUniModel{};
    GLint                          m_pieceUniColor{};
    GLint                          m_pieceUniTime{};
    GLint                          m_pieceUniPlayerMode{};
    std::array<LoadedMesh, 7>      m_pieceModels; // indexé par PieceType (0=None…6=King)
    GLuint                         m_texLight{};  // texture bois claire
    GLint                          m_uniMVP{};
    GLint                          m_uniColor{};
    GLint                          m_uniTexture{};
    GLint                          m_uniUseTexture{};
    GLint                          m_uniLightMode{};
    GLuint                         m_fbo{};
    GLuint                         m_fboTexture{};
    GLuint                         m_fboDepth{};
    CameraController               m_camera{};
    LightingManager                m_lighting{};
    glm::mat4                      m_projMatrix{};
    // Couleurs des cases (modifiables en mode chaos)
    glm::vec3 m_colorLight{1.f, 0.871f, 0.455f};
    glm::vec3 m_colorDark {0.804f, 0.510f, 0.247f};
    // Surbrillance des cases (§3.6)
    BoardHighlight                  m_highlight{};
    int                             m_hoverRow{-1};
    int                             m_hoverCol{-1};
    int                             m_selRow{-1};
    int                             m_selCol{-1};
    std::vector<std::pair<int,int>> m_validMoves;
};