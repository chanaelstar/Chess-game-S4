#pragma once
#include <string>
#include <vector>
#include "3D/Renderer.hpp"
#include "AI/AIPlayer.hpp"
#include "ChessBoard.hpp"
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
    bool shouldQuit() const { return m_shouldQuit; }

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
    void applyChaosEvent(); // Mode Infernal : téléporte une pièce aléatoire

    UniformDiscrete m_uniform;
};