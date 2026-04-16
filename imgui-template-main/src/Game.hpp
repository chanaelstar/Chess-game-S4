#pragma once
#include <string>
#include <vector>
#include "3D/Renderer.hpp"
#include "AI/AIPlayer.hpp"
#include "ChessBoard.hpp"
#include <chrono>
#include <vector>
#include "Maths/BetaDistribution.hpp"
#include "Maths/BinomialDistribution.hpp"
#include "Maths/CauchyDistribution.hpp"
#include "Maths/ExponentialDistribution.hpp"
#include "Maths/GeometricDistribution.hpp"
#include "Maths/NormalDistribution.hpp"
#include "Maths/PoissonDistribution.hpp"
#include "Maths/UniformDiscrete.hpp"
#include "Piece.hpp"
#include "UI/GameHUD.hpp"
#include "UI/Interface.hpp"


class Game {
public:
    Game();
    void init();
    void update();
    void onWindowResize(int width, int height);
    bool shouldQuit() const { return m_shouldQuit || m_interface.getAppState() == AppState::Exit; }

    struct TurnSnapshot {
        BoardSnapshot            board;
        PieceColor               currentPlayer;
        std::vector<std::string> moveHistory;
    };

private:
    Interface                 m_interface;
    ChessBoard                m_board;
    PieceColor                m_currentPlayer;
    PieceColor                m_winner = PieceColor::None;
    Renderer3D                m_renderer;
    bool                      m_shouldQuit{false};
    GameHUD                   m_hud;
    std::vector<std::string>  m_moveHistory;
    std::vector<TurnSnapshot> m_undoStack;

    void switchPlayer();
    void drawVictoryPopup();
    void applyChaosEvent(std::string& logEntry);
    void applyBinomialPawnSetup();
    void perturbColors(); // Dérive de couleur via N(0, 0.05²)             // Mode Infernal : retrait de pions au démarrage

    UniformDiscrete       m_uniform;
    GeometricDistribution m_geom;
    PoissonDistribution   m_poisson;
    BinomialDistribution  m_binomial;
    ExponentialDistribution m_expDist;
    NormalDistribution      m_normal;
    BetaDistribution        m_beta;
    CauchyDistribution      m_cauchy;
    int                   m_chaosCountdown{1};
    bool                  m_chaosInitialized{false};
    float                 m_spontaneousTimer{0.f};
    std::chrono::steady_clock::time_point m_lastFrameTime{};
    // Couleurs courantes du plateau — dérivent via N(0, 0.05²) à chaque événement
    glm::vec3             m_currentLight{1.f, 0.871f, 0.455f};
    glm::vec3             m_currentDark {0.804f, 0.510f, 0.247f};
};