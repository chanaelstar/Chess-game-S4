#include "Game.hpp"
#include <imgui.h>
#include <utility>
#include <vector>
#include "3D/Renderer.hpp"


Game::Game()
    : m_currentPlayer(PieceColor::White) {}

void Game::init()
{
    m_renderer.init();
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
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
            m_moveHistory.clear();
            m_undoStack.clear();
            m_currentPlayer = PieceColor::White;
            m_winner        = PieceColor::None;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Menu", ImVec2(80, 40)))
        {
            m_board.resetBoard();
            m_moveHistory.clear();
            m_undoStack.clear();
            m_currentPlayer = PieceColor::White;
            m_winner        = PieceColor::None;
            m_interface.backToMenu();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

static std::string formatMove(PieceColor color, const ChessBoard::LastMove& m)
{
    std::string who = (color == PieceColor::White) ? "Blancs" : "Noirs";
    char        fc  = 'a' + m.fromCol;
    char        tc  = 'a' + m.toCol;
    int         fr  = 8 - m.fromRow;
    int         tr  = 8 - m.toRow;
    return who + ": " + fc + std::to_string(fr) + " -> " + tc + std::to_string(tr);
}

void Game::applyChaosEvent(std::string& logEntry)
{
    // Collecte toutes les pièces non-Roi (pour ne pas téléporter les rois)
    std::vector<std::pair<int, int>> pieces;
    std::vector<std::pair<int, int>> emptySquares;

    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
        {
            const Piece* p = m_board.getPiece(r, c);
            if (p && p->getType() != PieceType::King)
                pieces.push_back({r, c});
            else if (!p)
                emptySquares.push_back({r, c});
        }

    if (pieces.empty() || emptySquares.empty())
        return;

    // X ~ U({0, ..., |pieces|-1}) : choisit la pièce à téléporter
    int pieceIdx            = m_uniform.sample(0, static_cast<int>(pieces.size()) - 1);
    auto [fromRow, fromCol] = pieces[pieceIdx];

    // Y ~ U({0, ..., |emptySquares|-1}) : choisit la case de destination
    int squareIdx       = m_uniform.sample(0, static_cast<int>(emptySquares.size()) - 1);
    auto [toRow, toCol] = emptySquares[squareIdx];

    const Piece* p = m_board.getPiece(fromRow, fromCol);
    logEntry       = std::string("[Chaos] ") + (char)('a' + fromCol) + std::to_string(8 - fromRow)
               + " -> " + (char)('a' + toCol) + std::to_string(8 - toRow);

    m_board.movePiece(fromRow, fromCol, toRow, toCol);
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
    // Sauvegarder l'état AVANT le coup (drawBoard applique le coup en interne)
    TurnSnapshot preMove{m_board.takeSnapshot(), m_currentPlayer, m_moveHistory};
    if (m_board.drawBoard(m_currentPlayer))
    {
        m_undoStack.push_back(preMove);

        if (auto lm = m_board.getLastMove())
            m_moveHistory.push_back(formatMove(m_currentPlayer, *lm));

        m_winner = m_board.getWinner();
        if (m_winner == PieceColor::None)
        {
            // Mode Infernal : événement chaos cadencé par une loi géométrique
            if (m_interface.getGameMode() == GameMode::RandomMode)
            {
                --m_chaosCountdown;
                if (m_chaosCountdown <= 0)
                {
                    std::string chaosLog;
                    applyChaosEvent(chaosLog);
                    if (!chaosLog.empty())
                        m_moveHistory.push_back(chaosLog);
                    // Prochain événement dans X ~ Geom(0.4) tours (E[X] = 2.5 tours)
                    m_chaosCountdown = m_geom.sample(0.4);
                }
            }

            m_winner = m_board.getWinner();
        }
        if (m_winner == PieceColor::None)
        {
            switchPlayer();
            if (m_interface.getGameMode() == GameMode::OnePlayer
                && m_currentPlayer == PieceColor::Black)
            {
                auto aiMove = AIPlayer::getMove(m_board, PieceColor::Black);
                if (aiMove)
                {
                    m_board.movePiece(aiMove->fromRow, aiMove->fromCol, aiMove->toRow, aiMove->toCol);
                    if (auto lm = m_board.getLastMove())
                        m_moveHistory.push_back(formatMove(PieceColor::Black, *lm));
                    m_winner = m_board.getWinner();
                    if (m_winner == PieceColor::None)
                        switchPlayer();
                }
            }
        }
    }

    if (m_hud.draw(m_currentPlayer, m_moveHistory, !m_undoStack.empty()) && !m_undoStack.empty())
    {
        auto& snap = m_undoStack.back();
        m_board.restoreSnapshot(snap.board);
        m_currentPlayer = snap.currentPlayer;
        m_moveHistory   = snap.moveHistory;
        m_winner        = PieceColor::None;
        m_undoStack.pop_back();
    }

    ImGui::SetNextWindowSize(ImVec2(Renderer3D::FBO_WIDTH, Renderer3D::FBO_HEIGHT + 20), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Vue 3D"))
    {
        ImVec2 avail  = ImGui::GetContentRegionAvail();
        float  aspect = (float)Renderer3D::FBO_WIDTH / Renderer3D::FBO_HEIGHT;
        float  drawW  = avail.x;
        float  drawH  = drawW / aspect;
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

        if (ImGui::IsItemHovered())
        {
            const ImGuiIO& io = ImGui::GetIO();
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                m_renderer.orbit(-io.MouseDelta.x * 0.005f, io.MouseDelta.y * 0.005f);
            if (io.MouseWheel != 0.0f)
                m_renderer.zoom(io.MouseWheel * 1.5f);
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
                m_renderer.pan(io.MouseDelta.x, io.MouseDelta.y);
        }
    }
    ImGui::End();

    if (m_winner != PieceColor::None)
        drawVictoryPopup();
}