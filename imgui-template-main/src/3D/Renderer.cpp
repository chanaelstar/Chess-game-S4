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
    m_projMatrix = glm::perspective(glm::radians(45.f), (float)FBO_WIDTH / FBO_HEIGHT, 0.1f, 100.f);
    m_viewMatrix = glm::lookAt(
        glm::vec3(0, 8, 6), // position caméra : au-dessus et légèrement derrière
        glm::vec3(0, 0, 0), // regarde vers le centre
        glm::vec3(0, 1, 0)  // "haut" = axe Y
    );

    // Création du FBO
    // Texture couleur
    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Renderbuffer depth
    glGenRenderbuffers(1, &m_fboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, FBO_WIDTH, FBO_HEIGHT);

    // FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer3D::draw(const ChessBoard& board)
{
    // Rendu dans le FBO
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // revenir au framebuffer par défaut
}

