#pragma once
#include <glm/glm.hpp>
#include <utility>

// Calcule l'intersection rayon-plan pour déterminer
// quelle case de l'échiquier est sous le curseur.
class RayCaster {
public:
    // mouseRel  : position de la souris relative au coin haut-gauche de l'image affichée
    // imageSize : dimensions (pixels) de l'image affichée à l'écran
    // proj, view: matrices de la caméra
    // Retourne {row, col} dans [0,7] ou {-1,-1} si hors plateau
    static std::pair<int,int> pickBoardSquare(
        glm::vec2       mouseRel,
        glm::vec2       imageSize,
        const glm::mat4& proj,
        const glm::mat4& view,
        float            boardY = 0.45f);
};
