#pragma once
#include <glm/glm.hpp>

// Gère l'animation de déplacement d'une pièce :
// translation progressive avec arc parabolique et easing smoothstep.
// Une seule animation active à la fois — un nouveau startMove() l'écrase.
class PieceAnimator {
public:
    // Lance l'animation de la pièce qui vient d'être déplacée.
    // duration : durée totale en secondes.
    void startMove(int fromRow, int fromCol, int toRow, int toCol, float duration = 0.45f);

    // Avance l'animation de dt secondes.
    void update(float dt);

    bool isAnimating() const { return m_active; }

    // Si (row, col) est la destination de la pièce en cours d'animation,
    // modifie worldPos avec la position interpolée et retourne true.
    // Sinon retourne false (worldPos inchangé).
    bool getAnimatedPos(int row, int col, glm::vec3& worldPos) const;

private:
    // Position monde du dessus de la case (row, col)
    static glm::vec3 squareTop(int row, int col);

    bool  m_active{false};
    int   m_fromRow{}, m_fromCol{};
    int   m_toRow{},   m_toCol{};
    float m_t{0.f};        // progression brute [0, 1]
    float m_duration{0.45f};
};
