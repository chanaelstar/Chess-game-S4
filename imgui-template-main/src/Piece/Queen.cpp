#include "Queen.hpp"
#include <cstdlib>

Queen::Queen(PieceColor color)
    : Piece(PieceType::Queen, color) {}

bool Queen::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const{
    int dRow = std::abs(toRow - fromRow);
    int dCol = std::abs(toCol - fromCol);

    // Mouvement en ligne droite ou diagonale
    if (dRow != 0 && dCol != 0 && dRow != dCol)
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