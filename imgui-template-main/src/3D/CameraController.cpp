#include "3D/CameraController.hpp"
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

CameraController::CameraController()
{
    recomputeViewMatrix();
}

// Trackball
void CameraController::orbit(float dTheta, float dPhi)
{
    m_theta += dTheta;
    const float TWO_PI = 2.f * glm::pi<float>();
    if (m_theta > glm::pi<float>())
        m_theta -= TWO_PI;
    if (m_theta < -glm::pi<float>())
        m_theta += TWO_PI;

    // phi clampé entre 2° et 89° → caméra jamais sous l'échiquier
    m_phi = glm::clamp(m_phi + dPhi, glm::radians(2.f), glm::radians(89.f));
    recomputeViewMatrix();
}

void CameraController::zoom(float delta)
{
    m_distance = glm::clamp(m_distance - delta, 3.f, 30.f);
    recomputeViewMatrix();
}

void CameraController::setDistance(float d)
{
    m_distance = glm::clamp(d, 3.f, 30.f);
    recomputeViewMatrix();
}

void CameraController::pan(float dx, float dy)
{
    glm::vec3 eye{
        m_target.x + m_distance * std::cos(m_phi) * std::sin(m_theta),
        m_target.y + m_distance * std::sin(m_phi),
        m_target.z + m_distance * std::cos(m_phi) * std::cos(m_theta)
    };
    glm::vec3 forward = glm::normalize(m_target - eye);
    glm::vec3 right   = glm::normalize(glm::cross(forward, glm::vec3(0.f, 1.f, 0.f)));
    glm::vec3 up      = glm::normalize(glm::cross(right, forward));
    float     scale   = m_distance * 0.01f;
    m_target -= right * (dx * scale);
    m_target += up * (dy * scale);
    recomputeViewMatrix();
}

// PieceView
void CameraController::lookAround(float dTheta, float dPhi)
{
    m_pvTheta += dTheta;
    m_pvPhi = glm::clamp(m_pvPhi + dPhi, glm::radians(-89.f), glm::radians(89.f));
    recomputeViewMatrix();
}

void CameraController::setPiecePosition(glm::vec3 worldTop)
{
    m_piecePos = worldTop;
    recomputeViewMatrix();
}

// Mode
void CameraController::setMode(CameraMode mode)
{
    m_mode = mode;
    if (mode == CameraMode::PieceView)
    {
        m_pvTheta = m_theta;
        m_pvPhi   = 0.f;
    }
    recomputeViewMatrix();
}

// Calcul de la matrice de vue
void CameraController::recomputeViewMatrix()
{
    if (m_mode == CameraMode::Trackball)
    {
        glm::vec3 eye{
            m_target.x + m_distance * std::cos(m_phi) * std::sin(m_theta),
            m_target.y + m_distance * std::sin(m_phi),
            m_target.z + m_distance * std::cos(m_phi) * std::cos(m_theta)
        };
        m_viewMatrix = glm::lookAt(eye, m_target, glm::vec3(0.f, 1.f, 0.f));
    }
    else // PieceView
    {
        glm::vec3 dir{
            std::cos(m_pvPhi) * std::sin(m_pvTheta),
            std::sin(m_pvPhi),
            std::cos(m_pvPhi) * std::cos(m_pvTheta)
        };
        // up vector de secours si on regarde presque verticalement
        glm::vec3 up = (std::abs(m_pvPhi) > glm::radians(80.f))
                           ? glm::vec3(std::sin(m_pvTheta + glm::pi<float>()), 0.f, std::cos(m_pvTheta + glm::pi<float>()))
                           : glm::vec3(0.f, 1.f, 0.f);
        m_viewMatrix = glm::lookAt(m_piecePos, m_piecePos + dir, up);
    }
}
