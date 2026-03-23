#include "King.hpp"
#include <cstdlib> // std::abs

King::King(PieceColor color)
    : Piece(PieceType::King, color) {}

bool King::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const
{
    int dRow = std::abs(toRow - fromRow);
    int dCol = std::abs(toCol - fromCol);

    // Une seule case dans n'importe quelle direction
    if (dRow > 1 || dCol > 1 || (dRow == 0 && dCol == 0))
        return false;

    // Destination vide ou adverse
    Piece* target = board[toRow][toCol];
    return target == nullptr
           || target->getColor() != getColor();
}