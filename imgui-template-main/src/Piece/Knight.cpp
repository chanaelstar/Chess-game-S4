#include "Knight.hpp"
#include <cstdlib>

Knight::Knight(PieceColor color)
    : Piece(PieceType::Knight, color) {}

bool Knight::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const{
    int dRow = std::abs(toRow - fromRow);
    int dCol = std::abs(toCol - fromCol);

    // Mouvement en "L" : 2 cases dans une direction, 1 dans l'autre
    if (!((dRow == 2 && dCol == 1) || (dRow == 1 && dCol == 2)))
        return false;

    // Destination vide ou adverse
    Piece* target = board[toRow][toCol];
    return target == nullptr
           || target->isEmpty()
           || target->getColor() != getColor();
}