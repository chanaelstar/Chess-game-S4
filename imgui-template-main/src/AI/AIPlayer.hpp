#pragma once
#include <optional>
#include <tuple>
#include "ChessBoard.hpp"
#include "Piece.hpp"

class ChessBoard;

struct Move {
    int fromRow, fromCol, toRow, toCol;
};

class AIPlayer {
public:
    // Retourne un coup valide pour la couleur donnée, ou nullopt si aucun coup possible
    static std::optional<Move> getMove(const ChessBoard& board, PieceColor color);
};