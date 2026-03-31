#include "AI/AIPlayer.hpp"
#include <vector>

std::optional<Move> AIPlayer::getMove(const ChessBoard& board, PieceColor color)
{
    std::vector<Move> allMoves;

    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
        {
            const Piece* p = board.getPiece(row, col);
            if (!p || p->getColor() != color)
                continue;
            for (auto [toRow, toCol] : board.getValidMoves(row, col))
                allMoves.push_back({row, col, toRow, toCol});
        }

    if (allMoves.empty())
        return std::nullopt;

    // Choix aléatoire
    int idx = std::rand() % static_cast<int>(allMoves.size());
    return allMoves[idx];
}