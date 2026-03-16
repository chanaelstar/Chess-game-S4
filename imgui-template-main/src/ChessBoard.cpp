#include "ChessBoard.hpp"
#include <imgui.h>
#include <algorithm>
#include <iostream>

ChessBoard::ChessBoard() : selectedRow(-1), selectedCol(-1)
{
    resetBoard();
}

void ChessBoard::setSelectedSquare(int row, int col)
{
    if (selectedRow == row && selectedCol == col)
    {
        selectedRow = selectedCol = -1;
    }
    else
    {
        selectedRow = row;
        selectedCol = col;
    }
}

void ChessBoard::movePiece(int fromRow, int fromCol, int toRow, int toCol)
{
    m_grid[toRow][toCol] = m_grid[fromRow][fromCol];
    m_grid[fromRow][fromCol] = Piece(PieceType::None, PieceColor::None);
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

void ChessBoard::drawBoard()
{
    ImGui::Begin("Chess board");
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
    const ImVec4 CHESS_LIGHT = ImVec4(1.0f, 0.871f, 0.455f, 1.0f);
    const ImVec4 CHESS_DARK = ImVec4(0.804f, 0.510f, 0.247f, 1.0f);
    const ImVec4 HIGHLIGHT = ImVec4(0.415f, 0.658f, 0.309f, 1.f);
    float boardSize = std::min(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    float button_size = boardSize / 8.0f;
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            Piece& piece = m_grid[row][col];
            bool isWhite = (row + col) % 2 == 0;
            bool isSelected = (row == selectedRow && col == selectedCol);
            ImVec4 background = isSelected ? HIGHLIGHT
                                           : (isWhite ? CHESS_LIGHT : CHESS_DARK);
            ImGui::PushStyleColor(ImGuiCol_Button, background);
            ImVec4 pieceColor = (piece.getColor() == PieceColor::Black)
                                    ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
                                    : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, pieceColor);
            ImGui::PushID(row * 8 + col);
            ImGui::PushFont(chessFont);
            ImGui::SetWindowFontScale(0.5);

if (ImGui::Button(piece.getSymbol(), ImVec2(button_size, button_size)))
{
    // Si aucune pièce n'est sélectionnée & la case actuelle a une pièce
    if (selectedRow == -1 && piece.getType() != PieceType::None)
    {
        setSelectedSquare(row, col);
        std::cout << "Piece selected: " << (8 - row) << (char)('a' + col) << "\n";
    }
    else if (selectedRow != -1) // pièce déjà sélectionnée
    {
        // Si on clique sur la même case: désélectionner
        if (selectedRow == row && selectedCol == col)
        {
            setSelectedSquare(-1, -1);
            std::cout << "Piece deselected\n";
        }
        else
        {
            // Déplacement de la pièce
            m_grid[row][col] = m_grid[selectedRow][selectedCol];
            m_grid[selectedRow][selectedCol] = Piece(PieceType::None, PieceColor::None);
            std::cout << "Piece moved to: " << (8 - row) << (char)('a' + col) << "\n";
            
            // Enlève la sélection après le déplacement
            setSelectedSquare(-1, -1);
        }
    }
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
