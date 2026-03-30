#include "Interface.hpp"

GameMode Interface::drawMenu()
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    float    btnWidth = 260.f;
    float    btnHeight = 55.f;

    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(340.f, 300.f), ImGuiCond_Always);
    ImGui::Begin(
        "##menu", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar
    );

    // Titre
    float titleWidth = ImGui::CalcTextSize("ECHECS").x;
    ImGui::SetCursorPosX((340.f - titleWidth) * 0.5f);
    ImGui::Text("ECHECS");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float btnX = (340.f - btnWidth) * 0.5f;

    // 1 joueur
    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("1 Joueur  (vs IA)", ImVec2(btnWidth, btnHeight)))
    {
        m_gameMode = GameMode::OnePlayer;
        m_appState = AppState::Playing;
    }

    ImGui::Spacing();

    // 2 joueurs
    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("2 Joueurs  (local)", ImVec2(btnWidth, btnHeight)))
    {
        m_gameMode = GameMode::TwoPlayers;
        m_appState = AppState::Playing;
    }

    ImGui::End();

    return m_gameMode;
}

void Interface::backToMenu()
{
    m_appState = AppState::Menu;
    m_gameMode = GameMode::None;
}
