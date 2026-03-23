#pragma once
#include "Piece.hpp"

class Knight : public Piece {
public:
    Knight(PieceColor color);

    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const override;
};