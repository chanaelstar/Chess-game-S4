#include "3D/Renderer.hpp"
#include <glimac/FilePath.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Renderer3D::buildBoardMesh()
{
    std::vector<float> vertices;

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            float x = col - 4.0f; // coin bas-gauche de la case
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

    // Position : 3 floats par vertex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

    // Caméra en perspective 3/4 : on regarde l'échiquier depuis le dessus légèrement incliné
    m_projMatrix = glm::perspective(glm::radians(45.f), (float)m_width / m_height, 0.1f, 100.f);
    m_viewMatrix = glm::lookAt(
        glm::vec3(0, 8, 6), // position caméra : au-dessus et légèrement derrière
        glm::vec3(0, 0, 0), // regarde vers le centre
        glm::vec3(0, 1, 0)  // "haut" = axe Y
    );
}

void Renderer3D::draw(const ChessBoard& board)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->use();
    glBindVertexArray(m_vao);

    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            // Couleur alternée blanc/noir
            bool      isLight = (row + col) % 2 == 0;
            glm::vec3 color   = isLight
                                    ? glm::vec3(1.f, 0.871f, 0.455f)     // clair (même couleur que ton ImGui)
                                    : glm::vec3(0.804f, 0.510f, 0.247f); // foncé

            glm::mat4 mvp = m_projMatrix * m_viewMatrix; // pas de model matrix, les cases sont déjà placées

            glUniformMatrix4fv(m_uniMVP, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform3fv(m_uniColor, 1, glm::value_ptr(color));

            // Chaque case = 6 vertices, à l'index (row*8 + col) * 6
            int firstVertex = (row * 8 + col) * 6;
            glDrawArrays(GL_TRIANGLES, firstVertex, 6);
        }
    }

    glBindVertexArray(0);
}

void Renderer3D::onWindowResize(int width, int height)
{
    m_width  = width;
    m_height = height;
    glViewport(0, 0, width, height);
    m_projMatrix = glm::perspective(glm::radians(45.f), (float)width / height, 0.1f, 100.f);
}
