#pragma once
#include "Piece.hpp"

class Rook : public Piece {
public:
    Rook(PieceColor color);

    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const override;
};