#include <array>
#include "Piece.hpp"

class ChessBoard {
private:
    std::array<std::array<Piece, 8>, 8> m_grid;

public:
    ChessBoard();
    void resetBoard();
    void drawBoard();
};
inline ImFont* chessFont = nullptr;