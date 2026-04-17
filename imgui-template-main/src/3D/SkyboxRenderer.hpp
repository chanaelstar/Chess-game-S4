#pragma once
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glm/glm.hpp>
#include <optional>

class SkyboxRenderer {
public:
    void init();
    void draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);
private:
    void buildMesh();
    GLuint m_vao{}, m_vbo{};
    std::optional<glimac::Program> m_program;
    GLint m_uniVP{};
};
