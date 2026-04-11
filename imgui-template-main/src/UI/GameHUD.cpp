#include "UI/GameHUD.hpp"
#include <imgui.h>

bool GameHUD::draw(PieceColor currentPlayer, const std::vector<std::string>& history, bool canUndo, int chaosCountdown)
{
    bool undoRequested = false;

    ImGui::Begin("Partie en cours");

    const char* tourText = (currentPlayer == PieceColor::White) ? "Tour des Blancs" : "Tour des Noirs";
    ImGui::Text("%s", tourText);

    ImGui::SameLine();
    if (!canUndo)
        ImGui::BeginDisabled();
    if (ImGui::Button("Annuler"))
        undoRequested = true;
    if (!canUndo)
        ImGui::EndDisabled();

    ImGui::Separator();

    ImGui::BeginChild("historique", ImVec2(0, 0), false);
    for (const auto& entry : history)
        ImGui::Text("%s", entry.c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::End();

    return undoRequested;
}
