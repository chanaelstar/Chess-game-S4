#include "3D/Renderer.hpp"
#include <cmath>
#include <glimac/FilePath.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static void addBox(std::vector<float>& verts, float x, float z, float w, float d, float h)
{
    const float x1 = x, x2 = x + w;
    const float y1 = 0.f, y2 = h;
    const float z1 = z, z2 = z + d;

    auto addFace = [&](float ax, float ay, float az,
                       float bx, float by, float bz,
                       float cx, float cy, float cz,
                       float dx, float dy, float dz) {
        verts.insert(verts.end(), {ax, ay, az, bx, by, bz, cx, cy, cz, ax, ay, az, cx, cy, cz, dx, dy, dz});
    };

    addFace(x1, y2, z1, x2, y2, z1, x2, y2, z2, x1, y2, z2); // dessus
    addFace(x1, y1, z1, x2, y1, z1, x2, y2, z1, x1, y2, z1); // avant
    addFace(x1, y1, z2, x1, y2, z2, x2, y2, z2, x2, y1, z2); // arrière
    addFace(x1, y1, z1, x1, y2, z1, x1, y2, z2, x1, y1, z2); // gauche
    addFace(x2, y1, z1, x2, y1, z2, x2, y2, z2, x2, y2, z1); // droite
    addFace(x1, y1, z1, x1, y1, z2, x2, y1, z2, x2, y1, z1); // dessous
}

void Renderer3D::buildBorderMesh()
{
    std::vector<float> verts;

    const float h = 0.45f;
    const float t = 0.3f; // épaisseur du bord

    addBox(verts, -4.f - t, -4.f - t, 8.f + 2.f * t, t, h); // avant
    addBox(verts, -4.f - t, 4.f, 8.f + 2.f * t, t, h);      // arrière
    addBox(verts, -4.f - t, -4.f, t, 8.f, h);               // gauche
    addBox(verts, 4.f, -4.f, t, 8.f, h);                    // droite

    m_borderVertexCount = (int)(verts.size() / 3);

    glGenVertexArrays(1, &m_borderVao);
    glGenBuffers(1, &m_borderVbo);

    glBindVertexArray(m_borderVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_borderVbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer3D::buildBoardMesh()
{
    std::vector<float> vertices;

    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
            addBox(vertices, col - 4.f, row - 4.f, 1.f, 1.f, 0.45f);

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
    if (m_theta > glm::pi<float>())
        m_theta -= TWO_PI;
    if (m_theta < -glm::pi<float>())
        m_theta += TWO_PI;
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
    m_target += up * (dy * scale);
    recomputeViewMatrix();
}

void Renderer3D::init()
{
    buildBoardMesh();
    buildBorderMesh();

    m_program = glimac::loadProgram(
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/board.vs.glsl",
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/board.fs.glsl"
    );
    m_program->use();
    m_uniMVP        = glGetUniformLocation(m_program->getGLId(), "uMVPMatrix");
    m_uniColor      = glGetUniformLocation(m_program->getGLId(), "uColor");
    m_uniTexture    = glGetUniformLocation(m_program->getGLId(), "uTexture");
    m_uniUseTexture = glGetUniformLocation(m_program->getGLId(), "uUseTexture");

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

    glm::mat4 mvp = m_projMatrix * m_viewMatrix;
    glUniformMatrix4fv(m_uniMVP, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(m_uniUseTexture, 0); // Utiliser les couleurs, pas les textures

    // Draw border
    glm::vec3 borderColor = glm::vec3(0.35f, 0.20f, 0.08f);
    glUniform3fv(m_uniColor, 1, glm::value_ptr(borderColor));
    glBindVertexArray(m_borderVao);
    glDrawArrays(GL_TRIANGLES, 0, m_borderVertexCount);

    // Draw chess squares
    glBindVertexArray(m_vao);
    int vertexIndex = 0;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            bool      isLight = (row + col) % 2 == 0;
            glm::vec3 color   = isLight
                                    ? glm::vec3(1.f, 0.871f, 0.455f)
                                    : glm::vec3(0.804f, 0.510f, 0.247f);

            glUniform3fv(m_uniColor, 1, glm::value_ptr(color));

            glDrawArrays(GL_TRIANGLES, vertexIndex, 36);
            vertexIndex += 36;
        }
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
