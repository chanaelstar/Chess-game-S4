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

private:
    void                           buildBoardMesh();
    GLuint                         m_vao{};
    GLuint                         m_vbo{};
    std::optional<glimac::Program> m_program;
    GLint                          m_uniMVP{};
    GLint                          m_uniColor{};
    GLuint                         m_fbo{};
    GLuint                         m_fboTexture{};
    GLuint                         m_fboDepth{};

    glm::mat4 m_projMatrix{};
    glm::mat4 m_viewMatrix{};
    // int       m_width{800};
    // int       m_height{800};
};