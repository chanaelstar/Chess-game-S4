#pragma once
#include "3D/Renderer.hpp"
#include "AI/AIPlayer.hpp"
#include "ChessBoard.hpp"
#include "Piece.hpp"
#include "UI/Interface.hpp"

class Game {
public:
    Game();
    void init();
    void update();
    void onWindowResize(int width, int height);
    bool shouldQuit() const { return m_shouldQuit; }

private:
    Interface  m_interface;
    ChessBoard m_board;
    PieceColor m_currentPlayer;
    PieceColor m_winner = PieceColor::None;
    Renderer3D m_renderer;
    bool       m_shouldQuit{false};
    AIPlayer   m_aiPlayer;

    void switchPlayer();
    void drawVictoryPopup();
};