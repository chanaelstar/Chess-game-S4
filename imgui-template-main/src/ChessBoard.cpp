#include "ChessBoard.hpp"
#include <imgui.h>
#include <algorithm>
#include <iostream>
#include "Piece/Bishop.hpp"
#include "Piece/King.hpp"
#include "Piece/Knight.hpp"
#include "Piece/Pawn.hpp"
#include "Piece/Queen.hpp"
#include "Piece/Rook.hpp"

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

void ChessBoard::removePiece(int row, int col)
{
    delete m_grid[row][col];
    m_grid[row][col] = nullptr;
}

void ChessBoard::movePiece(int fromRow, int fromCol, int toRow, int toCol)
{
    delete m_grid[toRow][toCol];
    m_grid[toRow][toCol]     = m_grid[fromRow][fromCol];
    m_grid[fromRow][fromCol] = nullptr;
    m_lastMove               = LastMove{fromRow, fromCol, toRow, toCol};
}

void ChessBoard::resetBoard()
{
    m_lastMove = std::nullopt;
    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
            m_grid[row][col] = nullptr;

    auto placeMainRow = [&](int row, PieceColor color) {
        m_grid[row][0] = new Rook(color);
        m_grid[row][1] = new Knight(color);
        m_grid[row][2] = new Bishop(color);
        m_grid[row][3] = new Queen(color);
        m_grid[row][4] = new King(color);
        m_grid[row][5] = new Bishop(color);
        m_grid[row][6] = new Knight(color);
        m_grid[row][7] = new Rook(color);
    };

    placeMainRow(0, PieceColor::Black);
    placeMainRow(7, PieceColor::White);
    for (int col = 0; col < 8; ++col)
    {
        m_grid[1][col] = new Pawn(PieceColor::Black);
        m_grid[6][col] = new Pawn(PieceColor::White);
    }
}
BoardSnapshot ChessBoard::takeSnapshot() const
{
    BoardSnapshot snap;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
        {
            if (m_grid[r][c])
                snap[r][c] = {m_grid[r][c]->getType(), m_grid[r][c]->getColor()};
            else
                snap[r][c] = {PieceType::None, PieceColor::None};
        }
    return snap;
}

Piece* ChessBoard::createPiece(PieceType type, PieceColor color)
{
    switch (type)
    {
    case PieceType::Pawn:   return new Pawn(color);
    case PieceType::Rook:   return new Rook(color);
    case PieceType::Knight: return new Knight(color);
    case PieceType::Bishop: return new Bishop(color);
    case PieceType::Queen:  return new Queen(color);
    case PieceType::King:   return new King(color);
    default:                return nullptr;
    }
}

void ChessBoard::restoreSnapshot(const BoardSnapshot& snapshot)
{
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
        {
            delete m_grid[r][c];
            m_grid[r][c] = createPiece(snapshot[r][c].first, snapshot[r][c].second);
        }
    selectedRow = selectedCol = -1;
    m_lastMove  = std::nullopt;
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

PieceColor ChessBoard::getWinner() const
{
    bool whiteKingAlive = false;
    bool blackKingAlive = false;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (m_grid[r][c] && m_grid[r][c]->getType() == PieceType::King)
            {
                if (m_grid[r][c]->getColor() == PieceColor::White)
                    whiteKingAlive = true;
                else
                    blackKingAlive = true;
            }
    if (!blackKingAlive)
        return PieceColor::White;
    if (!whiteKingAlive)
        return PieceColor::Black;
    return PieceColor::None;
}

bool ChessBoard::drawBoard(PieceColor currentPlayer)
{
    ImGui::Begin("Chess board");
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
    const ImVec4 CHESS_LIGHT = ImVec4(1.0f, 0.871f, 0.455f, 1.0f);
    const ImVec4 CHESS_DARK  = ImVec4(0.804f, 0.510f, 0.247f, 1.0f);
    const ImVec4 HIGHLIGHT   = ImVec4(0.415f, 0.658f, 0.309f, 1.0f);
    float        boardSize   = std::min(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    float        button_size = boardSize / 8.0f;

    const ImVec4                     VALID_MOVE = ImVec4(0.098f, 0.518f, 0.800f, 0.600f);
    std::vector<std::pair<int, int>> validMoves;
    bool                             moved = false;

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
            ImVec4 baseColor   = isWhite ? CHESS_LIGHT : CHESS_DARK;
            ImVec4 background  = isSelected ? HIGHLIGHT : baseColor;
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
                if (promotionRow == -1) // bloquer les interactions pendant la promotion
                {
                    if (selectedRow == -1 && piece != nullptr && piece->getColor() == currentPlayer)
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
                                moved = true;

                                // Vérifier si un pion a atteint la dernière rangée
                                Piece* moved = m_grid[row][col];
                                if (moved && moved->getType() == PieceType::Pawn)
                                {
                                    bool whitePromotion = (moved->getColor() == PieceColor::White && row == 0);
                                    bool blackPromotion = (moved->getColor() == PieceColor::Black && row == 7);
                                    if (whitePromotion || blackPromotion)
                                    {
                                        promotionRow = row;
                                        promotionCol = col;
                                    }
                                }
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
            }

            if (isValidMove)
            {
                ImVec2 buttonMin = ImGui::GetItemRectMin();
                ImVec2 buttonMax = ImGui::GetItemRectMax();
                ImGui::GetWindowDrawList()->AddRect(buttonMin, buttonMax, ImGui::GetColorU32(ImVec4(0.098f, 0.518f, 0.800f, 1.0f)), 0.0f, ImDrawFlags_None, 10.0f);
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

    // Ouvrir la modale si une promotion est en attente
    if (promotionRow != -1 && !ImGui::IsPopupOpen("Promotion"))
        ImGui::OpenPopup("Promotion");

    // Fenêtre modale de promotion du pion
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Promotion", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("Choisissez une piece pour la promotion :");
        ImGui::Separator();
        ImGui::Spacing();

        PieceColor color = m_grid[promotionRow][promotionCol]->getColor();

        auto promote = [&](Piece* newPiece) {
            delete m_grid[promotionRow][promotionCol];
            m_grid[promotionRow][promotionCol] = newPiece;
            promotionRow = promotionCol = -1;
            ImGui::CloseCurrentPopup();
        };

        if (ImGui::Button("Dame", ImVec2(110, 40)))
            promote(new Queen(color));
        ImGui::SameLine();
        if (ImGui::Button("Tour", ImVec2(110, 40)))
            promote(new Rook(color));
        ImGui::SameLine();
        if (ImGui::Button("Fou", ImVec2(110, 40)))
            promote(new Bishop(color));
        ImGui::SameLine();
        if (ImGui::Button("Cavalier", ImVec2(110, 40)))
            promote(new Knight(color));

        ImGui::EndPopup();
    }
    ImGui::End();
    return moved;
}
