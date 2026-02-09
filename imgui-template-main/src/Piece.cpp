#include "Piece.hpp"

Piece::Piece(PieceType type, PieceColor color)
    : m_type(type), m_color(color) {}

const char* Piece::getSymbol() const
{
    if (m_color == PieceColor::White)
    {
        switch (m_type)
        {
        case PieceType::Pawn: return "p";
        case PieceType::Rook: return "r";
        case PieceType::Knight: return "n";
        case PieceType::Bishop: return "b";
        case PieceType::Queen: return "q";
        case PieceType::King: return "k";
        default: return "";
        }
    }
    else if (m_color == PieceColor::Black)
    {
        switch (m_type)
        {
        case PieceType::Pawn: return "o";
        case PieceType::Rook: return "t";
        case PieceType::Knight: return "m";
        case PieceType::Bishop: return "v";
        case PieceType::Queen: return "w";
        case PieceType::King: return "l";
        default: return "";
        }
    }
    return "";
}