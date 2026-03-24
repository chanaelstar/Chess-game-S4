#pragma once
#include "ChessBoard.hpp"
#include "Piece.hpp"

class Game {
public:
    Game();
    void update();

private:
    ChessBoard m_board;
    PieceColor m_currentPlayer;

    void switchPlayer();
};