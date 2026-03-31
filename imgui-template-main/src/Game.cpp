#include "Game.hpp"
#include <imgui.h>
#include "3D/Renderer.hpp"

Game::Game()
    : m_currentPlayer(PieceColor::White) {}

void Game::init()
{
    m_renderer.init();
}
void Game::onWindowResize(int /*w*/, int /*h*/) {}

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
    m_renderer.draw(m_board);
    if (m_board.drawBoard(m_currentPlayer))
    {
        m_winner = m_board.getWinner();
        if (m_winner == PieceColor::None)
            switchPlayer();
    }

    ImGui::SetNextWindowSize(ImVec2(Renderer3D::FBO_WIDTH, Renderer3D::FBO_HEIGHT + 20), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Vue 3D"))
    {
        ImVec2    avail       = ImGui::GetContentRegionAvail();
        float     aspect      = (float)Renderer3D::FBO_WIDTH / Renderer3D::FBO_HEIGHT;
        float     drawW       = avail.x;
        float     drawH       = drawW / aspect;
        if (drawH > avail.y)
        {
            drawH = avail.y;
            drawW = drawH * aspect;
        }
        // Centrer l'image dans la zone disponible
        float offX = (avail.x - drawW) * 0.5f;
        float offY = (avail.y - drawH) * 0.5f;
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offX, ImGui::GetCursorPosY() + offY));
        ImGui::Image(
            (ImTextureID)(intptr_t)m_renderer.getTextureId(),
            ImVec2(drawW, drawH),
            ImVec2(0, 1), ImVec2(1, 0) // flip vertical (OpenGL vs ImGui convention)
        );
    }
    ImGui::End();

    if (m_winner != PieceColor::None)
        drawVictoryPopup();
}