#pragma once
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glm/glm.hpp>
#include <optional>
#include "3D/LightingManager.hpp"

class BoardRenderer {
public:
    void init();
    void draw(const glm::mat4& mvp, glm::vec3 colorLight, glm::vec3 colorDark,
              const LightingManager& lighting);
private:
    void buildBoardMesh();
    void buildBorderMesh();
    GLuint m_vao{}, m_vbo{};
    GLuint m_borderVao{}, m_borderVbo{};
    int    m_borderVertexCount{};
    GLuint m_texLight{};
    std::optional<glimac::Program> m_program;
    GLint m_uniMVP{}, m_uniColor{}, m_uniTexture{}, m_uniUseTexture{}, m_uniLightMode{};
};
