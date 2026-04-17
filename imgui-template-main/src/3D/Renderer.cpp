#include "3D/Renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ChessBoard.hpp"

std::pair<int, int> Renderer3D::pickSquare(float mx, float my, float imgW, float imgH) const
{
    return RayCaster::pickBoardSquare(
        {mx, my}, {imgW, imgH},
        m_projMatrix, m_camera.getViewMatrix()
    );
}

void Renderer3D::init()
{
    m_skybox.init();
    m_boardRenderer.init();
    m_pieceRenderer.init();
    m_highlight.init();
    m_lighting.start();

    glEnable(GL_DEPTH_TEST);
    m_projMatrix = glm::perspective(glm::radians(45.f), (float)FBO_WIDTH / FBO_HEIGHT, 0.1f, 100.f);

    // FBO
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

void Renderer3D::draw(const ChessBoard& board, float dt)
{
    m_animator.update(dt);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, FBO_WIDTH, FBO_HEIGHT);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Vue Pièce : la caméra suit la pièce pendant son animation
    if (m_camera.getMode() == CameraMode::PieceView && m_pvRow >= 0)
    {
        glm::vec3 base(m_pvCol - 4.f + 0.5f, 0.45f, m_pvRow - 4.f + 0.5f);
        m_animator.getAnimatedPos(m_pvRow, m_pvCol, base); // modifie base si pièce en vol
        base.y += m_pvHeightOffset;                        // sommet de la pièce
        m_camera.setPiecePosition(base);
    }

    const glm::mat4 viewMatrix = m_camera.getViewMatrix();
    const glm::mat4 mvp        = m_projMatrix * viewMatrix;

    m_skybox.draw(m_projMatrix, viewMatrix);
    m_boardRenderer.draw(mvp, m_colorLight, m_colorDark, m_lighting);
    m_highlight.draw(mvp, m_hoverRow, m_hoverCol, m_selRow, m_selCol, m_validMoves);
    m_pieceRenderer.draw(board, mvp, m_lighting, m_animator);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
