#include "3D/BoardHighlight.hpp"
#include <algorithm>
#include <glimac/FilePath.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static constexpr float OVERLAY_Y     = 0.452f;
static constexpr float SQUARE_MARGIN = 0.04f;

static const float QUAD_VERTS[] = {
    SQUARE_MARGIN,
    OVERLAY_Y,
    SQUARE_MARGIN,
    1.f - SQUARE_MARGIN,
    OVERLAY_Y,
    SQUARE_MARGIN,
    1.f - SQUARE_MARGIN,
    OVERLAY_Y,
    1.f - SQUARE_MARGIN,
    SQUARE_MARGIN,
    OVERLAY_Y,
    SQUARE_MARGIN,
    1.f - SQUARE_MARGIN,
    OVERLAY_Y,
    1.f - SQUARE_MARGIN,
    SQUARE_MARGIN,
    OVERLAY_Y,
    1.f - SQUARE_MARGIN,
};

void BoardHighlight::init()
{
    m_program = glimac::loadProgram(
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/overlay.vs.glsl",
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/overlay.fs.glsl"
    );
    m_uniMVP   = glGetUniformLocation(m_program->getGLId(), "uMVPMatrix");
    m_uniColor = glGetUniformLocation(m_program->getGLId(), "uColor");

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTS), QUAD_VERTS, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void BoardHighlight::drawSquare(const glm::mat4& mvp, int row, int col, const glm::vec4& color)
{
    glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(col - 4.f, 0.f, row - 4.f));
    glUniformMatrix4fv(m_uniMVP, 1, GL_FALSE, glm::value_ptr(mvp * model));
    glUniform4fv(m_uniColor, 1, glm::value_ptr(color));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void BoardHighlight::draw(
    const glm::mat4& mvp,
    int hoverRow, int hoverCol,
    int selectedRow, int selectedCol,
    const std::vector<std::pair<int, int>>& validMoves
)
{
    if (!m_program)
        return;
    m_program->use();
    glBindVertexArray(m_vao);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); // ne pas écrire dans le depth buffer

    // Coups valides (bleu) — dessinés en premier pour être sous le hover/select
    const glm::vec4 COLOR_VALID{0.20f, 0.50f, 1.00f, 0.45f};
    for (auto [r, c] : validMoves)
        drawSquare(mvp, r, c, COLOR_VALID);

    // Hover (jaune) — sélectionnable
    const glm::vec4 COLOR_HOVER{1.00f, 0.88f, 0.20f, 0.50f};
    if (hoverRow >= 0 && hoverRow != selectedRow || hoverCol != selectedCol)
        if (hoverRow >= 0)
            drawSquare(mvp, hoverRow, hoverCol, COLOR_HOVER);

    // Sélectionné (vert)
    const glm::vec4 COLOR_SELECTED{0.25f, 0.90f, 0.30f, 0.65f};
    if (selectedRow >= 0)
        drawSquare(mvp, selectedRow, selectedCol, COLOR_SELECTED);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
}
