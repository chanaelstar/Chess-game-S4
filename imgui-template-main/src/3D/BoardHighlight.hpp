#pragma once
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glm/glm.hpp>
#include <optional>
#include <utility>
#include <vector>

// Rendu des overlays de surbrillance sur les cases de l'échiquier.
// Trois états visuels :
// Hover (sélectionnable) = jaune
// Sélectionné = vert
// Coup valide = bleu
class BoardHighlight {
public:
    void init();
    void draw(const glm::mat4& mvp, int hoverRow, int hoverCol, int selectedRow, int selectedCol, const std::vector<std::pair<int, int>>& validMoves);

private:
    void drawSquare(const glm::mat4& mvp, int row, int col, const glm::vec4& color);

    GLuint                         m_vao{};
    GLuint                         m_vbo{};
    std::optional<glimac::Program> m_program;
    GLint                          m_uniMVP{};
    GLint                          m_uniColor{};
};
