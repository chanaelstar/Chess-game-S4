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
    ImVec4 m_colorLight{1.0f, 0.871f, 0.455f, 1.0f}; // couleur case claire (défaut)
    ImVec4 m_colorDark {0.804f, 0.510f, 0.247f, 1.0f}; // couleur case sombre (défaut)

public:
    ChessBoard();
    void                             resetBoard();
    bool                             drawBoard(PieceColor currentPlayer);
    void                             setSelectedSquare(int row, int col);
    void                             movePiece(int fromRow, int fromCol, int toRow, int toCol);
    void                             removePiece(int row, int col);
    void                             setChaosColors(ImVec4 light, ImVec4 dark);
    std::vector<std::pair<int, int>> getValidMoves(int row, int col) const;
    PieceColor                       getWinner() const;
    const Piece*                     getPiece(int row, int col) const { return m_grid[row][col]; }
    std::pair<int,int>               getSelectedSquare() const { return {selectedRow, selectedCol}; }
    std::optional<LastMove>          getLastMove() const { return m_lastMove; }
    BoardSnapshot                    takeSnapshot() const;
    void                             restoreSnapshot(const BoardSnapshot& snapshot);

private:
    static Piece* createPiece(PieceType type, PieceColor color);
};
inline ImFont* chessFont = nullptr;