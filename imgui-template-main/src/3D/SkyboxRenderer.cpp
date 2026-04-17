#include "3D/SkyboxRenderer.hpp"
#include <glm/gtc/type_ptr.hpp>

void SkyboxRenderer::buildMesh()
{
    // Cube unité [-1,1] — positions = directions de la skybox
    static constexpr float verts[] = {
        // Arrière
        -1.f,
        1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        // Avant
        -1.f,
        -1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        // Gauche
        -1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        // Droite
        1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        // Bas
        -1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        -1.f,
        // Haut
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SkyboxRenderer::init()
{
    buildMesh();
    m_program = glimac::loadProgram(
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/skybox.vs.glsl",
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/skybox.fs.glsl"
    );
    m_uniVP = glGetUniformLocation(m_program->getGLId(), "uViewProj");
}

void SkyboxRenderer::draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix)
{
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    m_program->use();
    glm::mat4 skyboxVP = projMatrix * glm::mat4(glm::mat3(viewMatrix));
    glUniformMatrix4fv(m_uniVP, 1, GL_FALSE, glm::value_ptr(skyboxVP));
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}
