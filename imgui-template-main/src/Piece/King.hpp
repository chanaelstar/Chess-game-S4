#pragma once
#include "Piece.hpp"

class King : public Piece {
public:
    King(PieceColor color);

    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const override;
};