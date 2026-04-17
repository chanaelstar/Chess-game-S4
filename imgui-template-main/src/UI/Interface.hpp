#pragma once
#include <imgui.h>
#include <cstdint>

enum class GameMode : std::uint8_t {
    None,
    OnePlayer,
    TwoPlayers,
    RandomMode,
};

enum class AppState : std::uint8_t {
    Menu,
    Playing,
    Exit,
};

class Interface {
private:
    AppState m_appState   = AppState::Menu;
    GameMode m_gameMode   = GameMode::None;
    GameMode m_randomMode = GameMode::None;

public:
    Interface() = default;

    // Affiche le menu principal, retourne le mode choisi
    GameMode drawMenu();

    AppState getAppState() const { return m_appState; }
    GameMode getGameMode() const { return m_gameMode; }
    GameMode getRandomMode() const { return m_randomMode; }

    void backToMenu();
};
