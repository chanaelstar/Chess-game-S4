#include "Interface.hpp"

GameMode Interface::drawMenu()
{
    ImGuiIO& io        = ImGui::GetIO();
    ImVec2   center    = ImGui::GetMainViewport()->GetCenter();
    float    btnWidth  = 260.f;
    float    btnHeight = 55.f;

    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(340.f, 300.f), ImGuiCond_Always);
    ImGui::Begin(
        "##menu", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar
    );

    // Titre
    float titleWidth = ImGui::CalcTextSize("CHESS").x;
    ImGui::SetCursorPosX((340.f - titleWidth) * 0.5f);
    ImGui::Text("CHESS");

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

    ImGui::Spacing();

    // Mode aléatoire
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.0f, 0.0f, 1.0f));        // Rouge sombre
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.1f, 0.1f, 1.0f)); // Rouge vif

    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("MODE INFERNAL", ImVec2(btnWidth, btnHeight)))
    {
        m_gameMode = GameMode::RandomMode;
        m_appState = AppState::Playing;
    }
    ImGui::PopStyleColor(2); // On remet les couleurs normales pour la suite

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Quitter
    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Quitter l'application", ImVec2(btnWidth, btnHeight)))
    {
        m_appState = AppState::Exit;
    }

    ImGui::End();

    return m_gameMode;
}

void Interface::backToMenu()
{
    m_appState = AppState::Menu;
    m_gameMode = GameMode::None;
}
