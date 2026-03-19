#pragma once

#include "Piece.hpp"

class Bishop : public Piece {
public:
    Bishop(PieceColor color);

    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const override;
};