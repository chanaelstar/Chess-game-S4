#pragma once
#include <imgui.h>
#include <array>
#include <optional>
#include <utility>
#include <vector>
#include "Piece.hpp"

using BoardSnapshot = std::array<std::array<std::pair<PieceType, PieceColor>, 8>, 8>;

class ChessBoard {
public:
    struct LastMove { int fromRow, fromCol, toRow, toCol; };

private:
    std::array<std::array<Piece*, 8>, 8> m_grid;
    int                                  selectedRow  = -1;
    int                                  selectedCol  = -1;
    int                                  promotionRow = -1;
    int                                  promotionCol = -1;
    std::optional<LastMove>              m_lastMove;

public:
    ChessBoard();
    void                             resetBoard();
    bool                             drawBoard(PieceColor currentPlayer);
    void                             setSelectedSquare(int row, int col);
    void                             movePiece(int fromRow, int fromCol, int toRow, int toCol);
    void                             removePiece(int row, int col);
    std::vector<std::pair<int, int>> getValidMoves(int row, int col) const;
    PieceColor                       getWinner() const;
    const Piece*                     getPiece(int row, int col) const { return m_grid[row][col]; }
    std::optional<LastMove>          getLastMove() const { return m_lastMove; }
    BoardSnapshot                    takeSnapshot() const;
    void                             restoreSnapshot(const BoardSnapshot& snapshot);

private:
    static Piece* createPiece(PieceType type, PieceColor color);
};
inline ImFont* chessFont = nullptr;