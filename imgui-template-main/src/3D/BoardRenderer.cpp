#include "3D/BoardRenderer.hpp"
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

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

void BoardRenderer::buildBorderMesh()
{
    std::vector<float> verts;
    const float        h = 0.45f, t = 0.3f;

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

void BoardRenderer::buildBoardMesh()
{
    std::vector<float> vertices;

    auto quad = [&](
                    float ax, float ay, float az, float au, float av,
                    float bx, float by, float bz, float bu, float bv,
                    float cx, float cy, float cz, float cu, float cv,
                    float dx, float dy, float dz, float du, float dv
                ) {
        vertices.insert(vertices.end(), {ax, ay, az, au, av, bx, by, bz, bu, bv, cx, cy, cz, cu, cv});
        vertices.insert(vertices.end(), {ax, ay, az, au, av, cx, cy, cz, cu, cv, dx, dy, dz, du, dv});
    };

    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
        {
            float x1 = col - 4.f, x2 = x1 + 1.f;
            float z1 = row - 4.f, z2 = z1 + 1.f;
            float y1 = 0.f, y2 = 0.45f;

            quad(x1, y2, z1, 0, 0, x2, y2, z1, 1, 0, x2, y2, z2, 1, 1, x1, y2, z2, 0, 1); // dessus
            quad(x1, y1, z1, 0, 0, x2, y1, z1, 1, 0, x2, y2, z1, 1, 1, x1, y2, z1, 0, 1); // avant
            quad(x2, y1, z2, 0, 0, x1, y1, z2, 1, 0, x1, y2, z2, 1, 1, x2, y2, z2, 0, 1); // arrière
            quad(x1, y1, z2, 0, 0, x1, y1, z1, 1, 0, x1, y2, z1, 1, 1, x1, y2, z2, 0, 1); // gauche
            quad(x2, y1, z1, 0, 0, x2, y1, z2, 1, 0, x2, y2, z2, 1, 1, x2, y2, z1, 0, 1); // droite
            quad(x1, y1, z2, 0, 0, x2, y1, z2, 1, 0, x2, y1, z1, 1, 1, x1, y1, z1, 0, 1); // dessous
        }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void BoardRenderer::init()
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
    m_uniLightMode  = glGetUniformLocation(m_program->getGLId(), "uLightMode");

    auto img = glimac::loadImage(glimac::FilePath(std::string(CMAKE_SOURCE_DIR) + "/assets/textures/oak-wood-texture.jpg"));
    if (img)
    {
        glGenTextures(1, &m_texLight);
        glBindTexture(GL_TEXTURE_2D, m_texLight);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, static_cast<GLsizei>(img->getWidth()), static_cast<GLsizei>(img->getHeight()), 0, GL_RGBA, GL_FLOAT, img->getPixels());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void BoardRenderer::draw(const glm::mat4& mvp, glm::vec3 colorLight, glm::vec3 colorDark, const LightingManager& lighting)
{
    m_program->use();
    glUniformMatrix4fv(m_uniMVP, 1, GL_FALSE, glm::value_ptr(mvp));
    lighting.applyToBoardShader(m_uniLightMode);

    // Bord (sans texture)
    glUniform1i(m_uniUseTexture, 0);
    glm::vec3 borderColor{0.35f, 0.20f, 0.08f};
    glUniform3fv(m_uniColor, 1, glm::value_ptr(borderColor));
    glBindVertexArray(m_borderVao);
    glDrawArrays(GL_TRIANGLES, 0, m_borderVertexCount);

    // Cases (texture + teinte couleur)
    glUniform1i(m_uniUseTexture, 1);
    glUniform1i(m_uniTexture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texLight);
    glBindVertexArray(m_vao);
    int vertexIndex = 0;
    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
        {
            glm::vec3 color = ((row + col) % 2 == 0) ? colorLight : colorDark;
            glUniform3fv(m_uniColor, 1, glm::value_ptr(color));
            glDrawArrays(GL_TRIANGLES, vertexIndex, 36);
            vertexIndex += 36;
        }
    glUniform1i(m_uniUseTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
