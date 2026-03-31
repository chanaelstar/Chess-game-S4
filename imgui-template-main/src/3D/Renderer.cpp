#include "3D/Renderer.hpp"
#include <glimac/FilePath.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Renderer3D::init()
{
    // VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_sphere.getVertexCount() * sizeof(glimac::ShapeVertex), m_sphere.getDataPointer(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Shaders
    m_program = glimac::loadProgram(
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/3D.vs.glsl",
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/normals.fs.glsl"
    );
    m_program->use();
    m_uniMVP    = glGetUniformLocation(m_program->getGLId(), "uMVPMatrix");
    m_uniMV     = glGetUniformLocation(m_program->getGLId(), "uMVMatrix");
    m_uniNormal = glGetUniformLocation(m_program->getGLId(), "uNormalMatrix");

    glEnable(GL_DEPTH_TEST);

    m_projMatrix = glm::perspective(glm::radians(70.f), (float)m_width / m_height, 0.1f, 100.f);
    m_mvMatrix   = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
}

void Renderer3D::draw(const ChessBoard& board)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 normalMatrix = glm::transpose(glm::inverse(m_mvMatrix));
    glm::mat4 mvpMatrix    = m_projMatrix * m_mvMatrix;

    glUniformMatrix4fv(m_uniMVP, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(m_uniMV, 1, GL_FALSE, glm::value_ptr(m_mvMatrix));
    glUniformMatrix4fv(m_uniNormal, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_sphere.getVertexCount());
    glBindVertexArray(0);
}

void Renderer3D::onWindowResize(int width, int height)
{
    m_width  = width;
    m_height = height;
    glViewport(0, 0, width, height);
    m_projMatrix = glm::perspective(glm::radians(70.f), (float)width / height, 0.1f, 100.f);
}