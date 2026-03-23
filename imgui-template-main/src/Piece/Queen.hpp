#pragma once
#include "Piece.hpp"

class Queen : public Piece {
public:
    Queen(PieceColor color);

    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const override;
};