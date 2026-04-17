#include "3D/PieceAnimator.hpp"
#include <algorithm>
#include <cmath>
#include <glm/gtx/compatibility.hpp> // glm::lerp / glm::mix

glm::vec3 PieceAnimator::squareTop(int row, int col)
{
    return glm::vec3(col - 4.f + 0.5f, 0.45f, row - 4.f + 0.5f);
}

void PieceAnimator::startMove(int fromRow, int fromCol, int toRow, int toCol, float duration, float postDelay)
{
    m_fromRow   = fromRow;
    m_fromCol   = fromCol;
    m_toRow     = toRow;
    m_toCol     = toCol;
    m_t         = 0.f;
    m_duration  = (duration > 0.f) ? duration : 0.45f;
    m_postDelay = (postDelay > 0.f) ? postDelay : 0.f;
    m_active    = true;
}

void PieceAnimator::update(float dt)
{
    if (m_active)
    {
        m_t += dt / m_duration;
        if (m_t >= 1.f)
        {
            m_t      = 1.f;
            m_active = false;
        }
    }
    else if (m_postDelay > 0.f)
    {
        m_postDelay -= dt;
        if (m_postDelay < 0.f)
            m_postDelay = 0.f;
    }
}

bool PieceAnimator::getAnimatedRotY(int row, int col, float& rotY) const
{
    if (!m_active || row != m_toRow || col != m_toCol)
        return false;

    glm::vec3 src = squareTop(m_fromRow, m_fromCol);
    glm::vec3 dst = squareTop(m_toRow, m_toCol);
    float     dx  = dst.x - src.x;
    float     dz  = dst.z - src.z;

    if (std::abs(dx) < 1e-4f && std::abs(dz) < 1e-4f)
        return false;

    rotY = std::atan2(dx, dz);
    return true;
}

bool PieceAnimator::getAnimatedPos(int row, int col, glm::vec3& worldPos) const
{
    if (!m_active || row != m_toRow || col != m_toCol)
        return false;

    glm::vec3 src = squareTop(m_fromRow, m_fromCol);
    glm::vec3 dst = squareTop(m_toRow, m_toCol);

    float t = m_t * m_t * (3.f - 2.f * m_t);

    worldPos = glm::mix(src, dst, t);

    float dist = std::sqrt((dst.x - src.x) * (dst.x - src.x) + (dst.z - src.z) * (dst.z - src.z));
    float arcH = std::max(0.25f, std::min(dist * 0.20f, 1.6f));

    worldPos.y += arcH * 4.f * m_t * (1.f - m_t);

    return true;
}
