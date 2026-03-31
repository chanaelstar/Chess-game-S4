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

    void init();
    void draw(const ChessBoard& board);
    void onWindowResize(int width, int height);

private:
    glimac::Sphere                 m_sphere{1, 32, 16};
    GLuint                         m_vao{};
    GLuint                         m_vbo{};
    std::optional<glimac::Program> m_program;
    GLint                          m_uniMVP{};
    GLint                          m_uniMV{};
    GLint                          m_uniNormal{};
    glm::mat4                      m_projMatrix{};
    glm::mat4                      m_mvMatrix{};
    int                            m_width{800};
    int                            m_height{800};
};