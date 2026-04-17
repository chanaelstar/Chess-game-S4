#pragma once
#include <glm/glm.hpp>

// Gère l'animation de déplacement d'une pièce :
// translation progressive avec arc parabolique et easing smoothstep.
// Une seule animation active à la fois — un nouveau startMove() l'écrase.
class PieceAnimator {
public:
    // Lance l'animation de la pièce qui vient d'être déplacée.
    // duration  : durée du déplacement en secondes.
    // postDelay : pause après l'atterrissage avant de signaler isReadyForNext().
    void startMove(int fromRow, int fromCol, int toRow, int toCol,
                   float duration = 0.45f, float postDelay = 0.35f);

    // Avance l'animation de dt secondes.
    void update(float dt);

    // Pièce en vol (position interpolée active).
    bool isAnimating() const { return m_active; }

    // Vrai quand le déplacement ET le délai post-animation sont terminés.
    // Indique que le jeu peut passer au joueur suivant.
    bool isReadyForNext() const { return !m_active && m_postDelay <= 0.f; }

    // Si (row, col) est la destination de la pièce en cours d'animation,
    // modifie worldPos avec la position interpolée et retourne true.
    // Sinon retourne false (worldPos inchangé).
    bool getAnimatedPos(int row, int col, glm::vec3& worldPos) const;

    // Retourne true et modifie rotY (radians, axe Y) si la pièce est en vol.
    // La pièce est orientée dans la direction de déplacement.
    bool getAnimatedRotY(int row, int col, float& rotY) const;

private:
    // Position monde du dessus de la case (row, col)
    static glm::vec3 squareTop(int row, int col);

    bool  m_active{false};
    int   m_fromRow{}, m_fromCol{};
    int   m_toRow{},   m_toCol{};
    float m_t{0.f};            // progression brute [0, 1]
    float m_duration{0.45f};
    float m_postDelay{0.f};    // délai résiduel après atterrissage
};
