#include "3D/Renderer.hpp"
#include <cmath>
#include <glimac/FilePath.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Renderer3D::buildBoardMesh()
{
    std::vector<float> vertices;

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            float x = col - 4.0f;
            float z = row - 4.0f;

            // Triangle 1
            vertices.insert(vertices.end(), {x, 0, z});
            vertices.insert(vertices.end(), {x + 1.f, 0, z});
            vertices.insert(vertices.end(), {x + 1.f, 0, z + 1.f});
            // Triangle 2
            vertices.insert(vertices.end(), {x, 0, z});
            vertices.insert(vertices.end(), {x + 1.f, 0, z + 1.f});
            vertices.insert(vertices.end(), {x, 0, z + 1.f});
        }
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer3D::recomputeViewMatrix()
{
    glm::vec3 eye;
    eye.x        = m_target.x + m_distance * std::cos(m_phi) * std::sin(m_theta);
    eye.y        = m_target.y + m_distance * std::sin(m_phi);
    eye.z        = m_target.z + m_distance * std::cos(m_phi) * std::cos(m_theta);
    m_viewMatrix = glm::lookAt(eye, m_target, glm::vec3(0.f, 1.f, 0.f));
}

void Renderer3D::orbit(float dTheta, float dPhi)
{
    m_theta += dTheta;
    const float TWO_PI = 2.0f * glm::pi<float>();
    if (m_theta >  glm::pi<float>()) m_theta -= TWO_PI;
    if (m_theta < -glm::pi<float>()) m_theta += TWO_PI;
    m_phi = glm::clamp(m_phi + dPhi, glm::radians(5.f), glm::radians(89.f));
    recomputeViewMatrix();
}

void Renderer3D::zoom(float delta)
{
    m_distance = glm::clamp(m_distance - delta, 3.0f, 30.0f);
    recomputeViewMatrix();
}

void Renderer3D::pan(float dx, float dy)
{
    glm::vec3 eye;
    eye.x             = m_target.x + m_distance * std::cos(m_phi) * std::sin(m_theta);
    eye.y             = m_target.y + m_distance * std::sin(m_phi);
    eye.z             = m_target.z + m_distance * std::cos(m_phi) * std::cos(m_theta);
    glm::vec3 forward = glm::normalize(m_target - eye);
    glm::vec3 right   = glm::normalize(glm::cross(forward, glm::vec3(0.f, 1.f, 0.f)));
    glm::vec3 up      = glm::normalize(glm::cross(right, forward));
    float     scale   = m_distance * 0.01f;
    m_target -= right * (dx * scale);
    m_target += up    * (dy * scale);
    recomputeViewMatrix();
}

void Renderer3D::init()
{
    buildBoardMesh();

    m_program = glimac::loadProgram(
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/board.vs.glsl",
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/board.fs.glsl"
    );
    m_program->use();
    m_uniMVP   = glGetUniformLocation(m_program->getGLId(), "uMVPMatrix");
    m_uniColor = glGetUniformLocation(m_program->getGLId(), "uColor");

    glEnable(GL_DEPTH_TEST);

    m_projMatrix = glm::perspective(glm::radians(45.f), (float)FBO_WIDTH / FBO_HEIGHT, 0.1f, 100.f);
    recomputeViewMatrix();

    // Création du FBO
    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &m_fboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, FBO_WIDTH, FBO_HEIGHT);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer3D::draw(const ChessBoard& board)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, FBO_WIDTH, FBO_HEIGHT);

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->use();
    glBindVertexArray(m_vao);

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            bool      isLight = (row + col) % 2 == 0;
            glm::vec3 color   = isLight
                                    ? glm::vec3(1.f, 0.871f, 0.455f)
                                    : glm::vec3(0.804f, 0.510f, 0.247f);

            glm::mat4 mvp = m_projMatrix * m_viewMatrix;

            glUniformMatrix4fv(m_uniMVP, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform3fv(m_uniColor, 1, glm::value_ptr(color));

            int firstVertex = (row * 8 + col) * 6;
            glDrawArrays(GL_TRIANGLES, firstVertex, 6);
        }
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
