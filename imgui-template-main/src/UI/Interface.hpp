#pragma once
#include <imgui.h>
#include <cstdint> // Pour les types uint8_t

enum class GameMode : std::uint8_t {
    None,
    OnePlayer,
    TwoPlayers,
};

enum class AppState : std::uint8_t {
    Menu,
    Playing,
};

class Interface {
public:
    Interface() = default;

    // Affiche le menu principal, retourne le mode choisi (None si aucun choix)
    GameMode drawMenu();

    AppState getAppState() const { return m_appState; }
    GameMode getGameMode() const { return m_gameMode; }

    void backToMenu();

private:
    AppState m_appState = AppState::Menu;
    GameMode m_gameMode = GameMode::None;
};
