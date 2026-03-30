#pragma once
#include "ChessBoard.hpp"
#include "Piece.hpp"
#include "UI/Interface.hpp"

class Game {
public:
    Game();
    void update();

private:
    Interface  m_interface;
    ChessBoard m_board;
    PieceColor m_currentPlayer;
    PieceColor m_winner = PieceColor::None;

    void switchPlayer();
    void drawVictoryPopup();
};