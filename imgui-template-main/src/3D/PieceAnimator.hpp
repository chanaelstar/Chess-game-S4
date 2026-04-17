#pragma once
#include <glm/glm.hpp>

// Gère l'animation de déplacement d'une pièce :

class PieceAnimator {
public:
    // Lance l'animation de la pièce qui vient d'être déplacée.
    // duration  : durée du déplacement en secondes.
    // postDelay : pause après l'atterrissage avant de signaler isReadyForNext().
    void startMove(int fromRow, int fromCol, int toRow, int toCol, float duration = 0.45f, float postDelay = 0.35f);

    void update(float dt);

    bool isAnimating() const { return m_active; }

    bool isReadyForNext() const { return !m_active && m_postDelay <= 0.f; }

    bool getAnimatedPos(int row, int col, glm::vec3& worldPos) const;

    bool getAnimatedRotY(int row, int col, float& rotY) const;

private:
    static glm::vec3 squareTop(int row, int col);

    bool  m_active{false};
    int   m_fromRow{}, m_fromCol{};
    int   m_toRow{}, m_toCol{};
    float m_t{0.f};
    float m_duration{0.45f};
    float m_postDelay{0.f};
};
