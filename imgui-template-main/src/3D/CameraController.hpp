#pragma once
#include <glm/glm.hpp>

enum class CameraMode { Trackball,
                        PieceView };

// Gère deux modes de caméra :
//  - Trackball : orbite autour du centre de l'échiquier, jamais sous le plateau
//  - PieceView : positionné au sommet d'une pièce, regard libre 360° sans limite
class CameraController {
public:
    CameraController();

    // Trackball
    void orbit(float dTheta, float dPhi);
    void zoom(float delta);
    void pan(float dx, float dy);
    void setDistance(float d);

    // PieceView
    void lookAround(float dTheta, float dPhi);
    void setPiecePosition(glm::vec3 worldTop);

    // Mode
    void       setMode(CameraMode mode);
    CameraMode getMode() const { return m_mode; }

    glm::mat4 getViewMatrix() const { return m_viewMatrix; }

private:
    void recomputeViewMatrix();

    CameraMode m_mode{CameraMode::Trackball};

    // Trackball
    float     m_theta{0.f};
    float     m_phi{glm::radians(53.13f)};
    float     m_distance{10.f};
    glm::vec3 m_target{0.f, 0.f, 0.f};

    // PieceView
    glm::vec3 m_piecePos{0.f, 1.f, 0.f};
    float     m_pvTheta{0.f};
    float     m_pvPhi{0.f};

    glm::mat4 m_viewMatrix{};
};
