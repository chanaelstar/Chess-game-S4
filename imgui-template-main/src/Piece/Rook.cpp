#include "Rook.hpp"
#include <cstdlib> // pour std::abs

Rook::Rook(PieceColor color)
    : Piece(PieceType::Rook, color) {}

bool Rook::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const
{
    // Doit se déplacer en ligne droite
    if (fromRow != toRow && fromCol != toCol)
        return false;

    int stepRow = (toRow > fromRow) ? 1 : (toRow < fromRow) ? -1 : 0;
    int stepCol = (toCol > fromCol) ? 1 : (toCol < fromCol) ? -1 : 0;

    // Cases intermédiaires — bloqué si occupé
    int r = fromRow + stepRow;
    int c = fromCol + stepCol;
    while (r != toRow || c != toCol)
    {
        if (board[r][c] != nullptr && !board[r][c]->isEmpty())
            return false;
        r += stepRow;
        c += stepCol;
    }

    // Destination : vide ou pièce adverse
    Piece* target = board[toRow][toCol];
    return target == nullptr
           || target->isEmpty()
           || target->getColor() != getColor();
}