#include "Game.hpp"

Game::Game() : m_currentPlayer(PieceColor::White) {}

void Game::switchPlayer()
{
    m_currentPlayer = (m_currentPlayer == PieceColor::White)
                          ? PieceColor::Black
                          : PieceColor::White;
}

void Game::update()
{
    if (m_board.drawBoard(m_currentPlayer))
        switchPlayer();
}