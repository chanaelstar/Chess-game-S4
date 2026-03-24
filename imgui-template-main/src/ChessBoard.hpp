#pragma once
#include <imgui.h>
#include <array>
#include <utility>
#include <vector>
#include "Piece.hpp"

class ChessBoard {
private:
    std::array<std::array<Piece*, 8>, 8> m_grid;
    int                                  selectedRow  = -1;
    int                                  selectedCol  = -1;
    int                                  promotionRow = -1;
    int                                  promotionCol = -1;

public:
    ChessBoard();
    void                             resetBoard();
    bool                             drawBoard(PieceColor currentPlayer);
    void                             setSelectedSquare(int row, int col);
    void                             movePiece(int fromRow, int fromCol, int toRow, int toCol);
    std::vector<std::pair<int, int>> getValidMoves(int row, int col) const;
};
inline ImFont* chessFont = nullptr;