#include "Bishop.hpp"
#include <cstdlib> // pour std::abs

Bishop::Bishop(PieceColor color)
    : Piece(PieceType::Bishop, color) {}

bool Bishop::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const
{
    int dRow = toRow - fromRow;
    int dCol = toCol - fromCol;

    // Doit se déplacer en diagonale
    if (std::abs(dRow) != std::abs(dCol) || dRow == 0)
        return false;

    int stepRow = (dRow > 0) ? 1 : -1;
    int stepCol = (dCol > 0) ? 1 : -1;

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