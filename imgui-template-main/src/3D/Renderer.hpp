#pragma once
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glimac/Sphere.hpp>
#include <glm/glm.hpp>
#include <optional>

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
    GLint                          m_uniMVP{};
    GLint                          m_uniColor{};
    GLint                          m_uniTexture{};
    GLint                          m_uniUseTexture{};
    GLuint                         m_fbo{};
    GLuint                         m_fboTexture{};
    GLuint                         m_fboDepth{};

    glm::mat4 m_projMatrix{};
    glm::mat4 m_viewMatrix{};
    // Coordonnées sphériques de la caméra
    float     m_theta{0.0f};               // angle horizontal (rad)
    float     m_phi{glm::radians(53.13f)}; // angle vertical (rad) ≈ atan2(8,6)
    float     m_distance{10.0f};           // distance au centre
    glm::vec3 m_target{0.f, 0.f, 0.f};     // point visé
};