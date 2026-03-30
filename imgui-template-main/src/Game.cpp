#include "Game.hpp"
#include <imgui.h>

Game::Game() : m_currentPlayer(PieceColor::White) {}

void Game::switchPlayer()
{
    m_currentPlayer = (m_currentPlayer == PieceColor::White)
                          ? PieceColor::Black
                          : PieceColor::White;
}

void Game::drawVictoryPopup()
{
    if (!ImGui::IsPopupOpen("Victoire"))
        ImGui::OpenPopup("Victoire");

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Victoire", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        const char* winner = (m_winner == PieceColor::White) ? "Blancs" : "Noirs";
        ImGui::Text("Les %s ont gagne !", winner);
        ImGui::Spacing();
        if (ImGui::Button("Nouvelle partie", ImVec2(150, 40)))
        {
            m_board.resetBoard();
            m_currentPlayer = PieceColor::White;
            m_winner        = PieceColor::None;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Menu", ImVec2(80, 40)))
        {
            m_board.resetBoard();
            m_currentPlayer = PieceColor::White;
            m_winner        = PieceColor::None;
            m_interface.backToMenu();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

// Quand fin de partie
void Game::update()
{
    if (m_interface.getAppState() == AppState::Menu)
    {
        m_interface.drawMenu();
        return;
    }

    if (m_board.drawBoard(m_currentPlayer))
    {
        m_winner = m_board.getWinner();
        if (m_winner == PieceColor::None)
            switchPlayer();
    }

    if (m_winner != PieceColor::None)
        drawVictoryPopup();
}