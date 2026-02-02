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

    ImVec2 avail       = ImGui::GetContentRegionAvail();
    float  boardWidth  = avail.x;
    float  boardHeight = avail.y;
    float  boardSize   = std::min(boardWidth, boardHeight);
    float  button_size = boardSize / 8.0f;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            bool   isWhite = (row + col) % 2 == 0;
            ImVec4 color   = isWhite ? CHESS_LIGHT : CHESS_DARK;
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushID(row * 8 + col);

            const char* label      = "";
            ImVec4      pieceColor = ImVec4(1.f, 1.f, 1.f, 1.f); // Blanc par défaut

            if (row == 0 || row == 1)
            {
                pieceColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Texte Noir pur
                if (row == 1)
                    label = "o"; // Pion noir
                else if (col == 0 || col == 7)
                    label = "t"; // Tour noire
                else if (col == 1 || col == 6)
                    label = "m"; // Cavalier noir
                else if (col == 2 || col == 5)
                    label = "v"; // Fou noir
                else if (col == 3)
                    label = "w"; // Dame noire
                else if (col == 4)
                    label = "l"; // Roi noir
            }

            else if (row == 6 || row == 7)
            {
                pieceColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Texte Blanc pur
                if (row == 6)
                    label = "p"; // Pion blanc
                else if (col == 0 || col == 7)
                    label = "r"; // Tour blanche
                else if (col == 1 || col == 6)
                    label = "n"; // Cavalier blanc
                else if (col == 2 || col == 5)
                    label = "b"; // Fou blanc
                else if (col == 3)
                    label = "q"; // Dame blanche
                else if (col == 4)
                    label = "k"; // Roi blanc
            }

            ImGui::PushStyleColor(ImGuiCol_Text, pieceColor);
            ImGui::SetWindowFontScale(0.5);
            ImGui::PushFont(chessFont);

            if (ImGui::Button(label, ImVec2(button_size, button_size)))
            {
                char colLetter = 'a' + col;
                int  rowNumber = 8 - row;
                std::cout << "Clicked square: " << rowNumber << colLetter << "\n";
            }
            ImGui::PopID();
            ImGui::PopStyleColor(2);
            ImGui::PopFont();
            ImGui::SetWindowFontScale(1.f);

            if (col < 7)
            {
                ImGui::SameLine();
            }
        }
    }
    ImGui::PopStyleVar(2);
    ImGui::End();
}