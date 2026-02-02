#include "ChessBoard.hpp"
#include <imgui.h>
#include <algorithm>
#include <iostream>

void ChessBoard::drawBoard()
{
    ImGui::Begin("Chess board");

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));

    const ImVec4 CHESS_LIGHT = ImVec4(0.93f, 0.93f, 0.82f, 1.0f);
    const ImVec4 CHESS_DARK  = ImVec4(0.46f, 0.59f, 0.34f, 1.0f);

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

            ImGui::PushFont(chessFont);
            const char* label = "";
            if (row == 1)
                label = "o"; // Pion noir
            if (row == 6)
                label = "p"; // Pion blanc
            if (row == 0 && (col == 0 || col == 7))
                label = "t"; // Tours noires

            if (ImGui::Button(label, ImVec2(button_size, button_size)))
            {
                char colLetter = 'a' + col;
                int  rowNumber = 8 - row;
                std::cout << "Clicked square: " << rowNumber << colLetter << "\n";
            }
            ImGui::PopID();
            ImGui::PopStyleColor();
            ImGui::PopFont();

            if (col < 7)
            {
                ImGui::SameLine();
            }
        }
    }
    ImGui::PopStyleVar(2);
    ImGui::End();
}