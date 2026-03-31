#pragma once
#include <string>
#include <vector>
#include "Piece.hpp"

class GameHUD {
public:
    bool draw(PieceColor currentPlayer, const std::vector<std::string>& history, bool canUndo);
};
