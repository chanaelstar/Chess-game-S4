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
    void                           buildBoardMesh();
    GLuint                         m_vao{};
    GLuint                         m_vbo{};
    std::optional<glimac::Program> m_program;
    GLint                          m_uniMVP{};
    GLint                          m_uniColor{};

    glm::mat4 m_projMatrix{};
    glm::mat4 m_viewMatrix{};
    int       m_width{800};
    int       m_height{800};
};