#include "ChessBoard.hpp"
#include <imgui.h>
#include <algorithm>
#include <iostream>

void ChessBoard::drawBoard()
{
    ImGui::Begin("Chess board");

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));

    const ImVec4 CHESS_LIGHT = ImVec4(1.0f, 0.871f, 0.455f, 1.0f);
    const ImVec4 CHESS_DARK  = ImVec4(0.804f, 0.510f, 0.247f, 1.0f);

    float boardSize   = std::min(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    float button_size = boardSize / 8.0f;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            Piece& piece = m_grid[row][col];

            bool isWhite = (row + col) % 2 == 0;
            ImGui::PushStyleColor(ImGuiCol_Button, isWhite ? CHESS_LIGHT : CHESS_DARK);

            ImVec4 pieceColor = (piece.getColor() == PieceColor::Black)
                                    ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
                                    : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, pieceColor);

            ImGui::PushID(row * 8 + col);
            ImGui::PushFont(chessFont);
            ImGui::SetWindowFontScale(0.5);

            if (ImGui::Button(piece.getSymbol(), ImVec2(button_size, button_size)))
            {
                std::cout << "Clicked: " << (8 - row) << (char)('a' + col) << "\n";
            }

            ImGui::PopFont();
            ImGui::PopID();
            ImGui::PopStyleColor(2);
            ImGui::SetWindowFontScale(1.f);

            if (col < 7)
                ImGui::SameLine();
        }
    }
    ImGui::PopStyleVar(2);
    ImGui::End();
}
ChessBoard::ChessBoard()
{
    resetBoard();
}
void ChessBoard::resetBoard()
{
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            m_grid[row][col] = Piece(PieceType::None, PieceColor::None);
        }
    }

    PieceType mainRow[] = {
        PieceType::Rook, PieceType::Knight, PieceType::Bishop, PieceType::Queen,
        PieceType::King, PieceType::Bishop, PieceType::Knight, PieceType::Rook
    };

    for (int col = 0; col < 8; ++col)
    {
        m_grid[0][col] = Piece(mainRow[col], PieceColor::Black);
        m_grid[1][col] = Piece(PieceType::Pawn, PieceColor::Black);
        m_grid[6][col] = Piece(PieceType::Pawn, PieceColor::White);
        m_grid[7][col] = Piece(mainRow[col], PieceColor::White);
    }
}