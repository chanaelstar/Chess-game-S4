#pragma once
#include <glm/glm.hpp>
#include <utility>

// Calcule l'intersection rayon-plan pour déterminer
// quelle case de l'échiquier est sous le curseur.
class RayCaster {
public:
    static std::pair<int, int> pickBoardSquare(
        glm::vec2        mouseRel,
        glm::vec2        imageSize,
        const glm::mat4& proj,
        const glm::mat4& view,
        float            boardY = 0.45f
    );
};
