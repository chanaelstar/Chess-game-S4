#pragma once
#include <imgui.h>

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

    PieceType  getType() const { return m_type; }
    PieceColor getColor() const { return m_color; }
    bool       isEmpty() const { return m_type == PieceType::None; }

    const char* getSymbol() const;

private:
    PieceType  m_type;
    PieceColor m_color;
};