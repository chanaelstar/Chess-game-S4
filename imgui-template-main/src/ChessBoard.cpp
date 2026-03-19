#include "ChessBoard.hpp"
#include <imgui.h>
#include <algorithm>
#include <iostream>
#include "Piece/Bishop.hpp"

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
    delete m_grid[toRow][toCol];
    m_grid[toRow][toCol]     = m_grid[fromRow][fromCol];
    m_grid[fromRow][fromCol] = nullptr;
}

void ChessBoard::resetBoard()
{
    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
            m_grid[row][col] = nullptr;
    {
        m_grid[0][2] = new Bishop(PieceColor::Black);
        m_grid[0][5] = new Bishop(PieceColor::Black);
        m_grid[7][2] = new Bishop(PieceColor::White);
        m_grid[7][5] = new Bishop(PieceColor::White);
        // Ici ajouter les autres pièces lors de l'implémentation
    }
}
std::vector<std::pair<int, int>> ChessBoard::getValidMoves(int row, int col) const
{
    std::vector<std::pair<int, int>> moves;
    Piece*                           piece = m_grid[row][col];
    if (!piece)
        return moves;

    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (piece->isValidMove(row, col, r, c, m_grid))
                moves.push_back({r, c});

    return moves;
}

void ChessBoard::drawBoard()
{
    ImGui::Begin("Chess board");
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
    const ImVec4 CHESS_LIGHT = ImVec4(1.0f, 0.871f, 0.455f, 1.0f);
    const ImVec4 CHESS_DARK  = ImVec4(0.804f, 0.510f, 0.247f, 1.0f);
    const ImVec4 HIGHLIGHT   = ImVec4(0.415f, 0.658f, 0.309f, 1.f);
    float        boardSize   = std::min(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    float        button_size = boardSize / 8.0f;

    const ImVec4                     VALID_MOVE = ImVec4(0.5f, 0.7f, 1.0f, 0.6f);
    std::vector<std::pair<int, int>> validMoves;
    if (selectedRow != -1)
        validMoves = getValidMoves(selectedRow, selectedCol);

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            Piece* piece = m_grid[row][col];

            bool isWhite    = (row + col) % 2 == 0;
            bool isSelected = (row == selectedRow && col == selectedCol);

            bool   isValidMove = std::find(validMoves.begin(), validMoves.end(), std::make_pair(row, col)) != validMoves.end();
            ImVec4 background  = isSelected    ? HIGHLIGHT
                                 : isValidMove ? VALID_MOVE
                                               : (isWhite ? CHESS_LIGHT : CHESS_DARK);
            ImGui::PushStyleColor(ImGuiCol_Button, background);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, background);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, background);

            // Couleur du symbole
            ImVec4 pieceColor = (piece && piece->getColor() == PieceColor::Black)
                                    ? ImVec4(0.f, 0.f, 0.f, 1.f)
                                    : ImVec4(1.f, 1.f, 1.f, 1.f);
            ImGui::PushStyleColor(ImGuiCol_Text, pieceColor);
            ImGui::PushID(row * 8 + col);
            ImGui::PushFont(chessFont);
            ImGui::SetWindowFontScale(0.5f);

            const char* symbol = piece ? piece->getSymbol() : "";

            if (ImGui::Button(symbol, ImVec2(button_size, button_size)))
            {
                if (selectedRow == -1 && piece != nullptr)
                {
                    setSelectedSquare(row, col);
                }
                else if (selectedRow != -1)
                {
                    if (selectedRow == row && selectedCol == col)
                    {
                        setSelectedSquare(-1, -1);
                    }
                    else
                    {
                        Piece* moving = m_grid[selectedRow][selectedCol];
                        if (moving->isValidMove(selectedRow, selectedCol, row, col, m_grid))
                        {
                            movePiece(selectedRow, selectedCol, row, col);
                            std::cout << "Piece moved to: " << (8 - row) << (char)('a' + col) << "\n";
                        }
                        else
                        {
                            std::cout << "Invalid move!\n";
                        }
                        setSelectedSquare(-1, -1);
                    }
                }
            }

            ImGui::PopFont();
            ImGui::PopID();
            ImGui::PopStyleColor(4);
            ImGui::SetWindowFontScale(1.f);

            if (col < 7)
                ImGui::SameLine();
        }
    }

    ImGui::PopStyleVar(2);
    ImGui::End();
}
