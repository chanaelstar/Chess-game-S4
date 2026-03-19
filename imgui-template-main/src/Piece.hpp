#pragma once
#include <array>

class Piece;
using Board = std::array<std::array<Piece*, 8>, 8>;

enum class PieceType { None,
                       Pawn,
                       Rook,
                       Knight,
                       Bishop,
                       Queen,
                       King };
enum class PieceColor { None,
                        White,
                        Black };

class Piece {
public:
    Piece(PieceType type = PieceType::None, PieceColor color = PieceColor::None);
    virtual ~Piece() = default;

    PieceType  getType() const { return m_type; }
    PieceColor getColor() const { return m_color; }
    bool       isEmpty() const { return m_type == PieceType::None; }

    const char* getSymbol() const;

    virtual bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const = 0;

private:
    PieceType  m_type;
    PieceColor m_color;
};