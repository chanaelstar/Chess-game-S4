#include "Pawn.hpp"
#include <cstdlib>

Pawn::Pawn(PieceColor color)
    : Piece(PieceType::Pawn, color) {}

bool Pawn::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const{
    int direction = (getColor() == PieceColor::White) ? -1 : 1;
    int startRow  = (getColor() == PieceColor::White) ? 6 : 1;

    // Mouvement de base : une case en avant
    if (toCol == fromCol && toRow == fromRow + direction)
    {
        return board[toRow][toCol] == nullptr;
    }

    // Premier mouvement : deux cases en avant
    if (toCol == fromCol && toRow == fromRow + 2 * direction && fromRow == startRow)
    {
        return board[fromRow + direction][toCol] == nullptr && board[toRow][toCol] == nullptr;
    }

    // Capture : une case en diagonale
    if (std::abs(toCol - fromCol) == 1 && toRow == fromRow + direction)
    {
        Piece* target = board[toRow][toCol];
        return target != nullptr && target->getColor() != getColor();
    }

    return false;
}