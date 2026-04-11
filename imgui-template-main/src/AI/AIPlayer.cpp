#include "AI/AIPlayer.hpp"
#include <vector>
#include "Maths/UniformDiscrete.hpp"


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

    // Loi uniforme discrète sur {0, ..., n-1} : chaque coup légal est équiprobable
    static UniformDiscrete ud;
    int                    idx = ud.sample(0, static_cast<int>(allMoves.size()) - 1);
    return allMoves[idx];
}