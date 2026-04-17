#pragma once
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glimac/Sphere.hpp>
#include <glm/glm.hpp>
#include <array>
#include <optional>
#include "3D/LightingManager.hpp"
#include "3D/ModelLoader.hpp"
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

    void orbit(float dTheta, float dPhi);
    void zoom(float delta);
    void pan(float dx, float dy);
    void setDistance(float d);
    void setChaosColors(glm::vec3 light, glm::vec3 dark);
    void setCurrentPlayer(PieceColor p) { m_lighting.setCurrentPlayer(p); }

private:
    void                           buildBoardMesh();
    void                           buildBorderMesh();
    void                           buildPieceMesh();
    void                           buildSkyboxMesh();
    void                           recomputeViewMatrix();
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
    LightingManager                m_lighting{};

    glm::mat4 m_projMatrix{};
    glm::mat4 m_viewMatrix{};
    // Coordonnées sphériques de la caméra
    float     m_theta{0.0f};               // angle horizontal (rad)
    float     m_phi{glm::radians(53.13f)}; // angle vertical (rad) ≈ atan2(8,6)
    float     m_distance{10.0f};           // distance au centre
    glm::vec3 m_target{0.f, 0.f, 0.f};    // point visé
    // Couleurs des cases (modifiables en mode chaos)
    glm::vec3 m_colorLight{1.f, 0.871f, 0.455f};
    glm::vec3 m_colorDark {0.804f, 0.510f, 0.247f};
};