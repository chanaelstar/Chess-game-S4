#include "Game.hpp"
#include <imgui.h>
#include <utility>
#include <vector>
#include <glm/glm.hpp>
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

void Game::resetGame()
{
    m_board.resetBoard();
    m_moveHistory.clear();
    m_undoStack.clear();
    m_pendingAIMove.reset();
    m_switchAfterAnim  = false;
    m_currentPlayer    = PieceColor::White;
    m_winner           = PieceColor::None;
    m_paused           = false;
    m_chaosInitialized = false;
    m_chaosCountdown   = 1;
    m_spontaneousTimer = 0.f;
    m_currentLight     = {1.f, 0.871f, 0.455f};
    m_currentDark      = {0.804f, 0.510f, 0.247f};
    m_board.setChaosColors({1.0f, 0.871f, 0.455f, 1.f}, {0.804f, 0.510f, 0.247f, 1.f});
    m_renderer.setChaosColors({1.f, 0.871f, 0.455f}, {0.804f, 0.510f, 0.247f});
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
            resetGame();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Menu", ImVec2(80, 40)))
        {
            resetGame();
            m_interface.backToMenu();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Game::drawPauseMenu()
{
    if (!ImGui::IsPopupOpen("Pause"))
        ImGui::OpenPopup("Pause");

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Pause", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("Jeu en pause");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Reprendre", ImVec2(200, 40)))
        {
            m_paused = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::Spacing();

        if (ImGui::Button("Menu principal", ImVec2(200, 40)))
        {
            resetGame();
            m_interface.backToMenu();
            ImGui::CloseCurrentPopup();
        }

        ImGui::Spacing();

        if (ImGui::Button("Quitter l'application", ImVec2(200, 40)))
        {
            m_shouldQuit = true;
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

void Game::applyBinomialPawnSetup()
{
    std::vector<bool> whiteTrials, blackTrials;
    int               removedWhite = m_binomial.sample(8, 0.3, whiteTrials);
    int               removedBlack = m_binomial.sample(8, 0.3, blackTrials);

    for (int col = 0; col < 8; ++col)
    {
        if (whiteTrials[col])
            m_board.removePiece(6, col);
        if (blackTrials[col])
            m_board.removePiece(1, col);
    }

    // Couleurs du plateau : chaque composante RGB tirée selon Beta
    // Cases claires : Beta(6, 2) → E = 0.75 (couleurs vives)
    // Cases sombres : Beta(2, 6) → E = 0.25 (couleurs sombres)
    float lr = m_beta.sample(6, 2), lg = m_beta.sample(6, 2), lb = m_beta.sample(6, 2);
    float dr = m_beta.sample(2, 6), dg = m_beta.sample(2, 6), db = m_beta.sample(2, 6);
    m_currentLight = {lr, lg, lb};
    m_currentDark  = {dr, dg, db};
    m_board.setChaosColors({lr, lg, lb, 1.f}, {dr, dg, db, 1.f});
    m_renderer.setChaosColors({lr, lg, lb}, {dr, dg, db});

    std::string log = "[Debut] B(8, 0.3) : "
                      + std::to_string(removedWhite) + " pion(s) blanc(s) et "
                      + std::to_string(removedBlack) + " pion(s) noir(s) absents";
    m_moveHistory.push_back(log);
}

void Game::perturbColors()
{
    // Dérive de couleur : chaque composante RGB ~ N(0, 0.05²)
    // Les perturbations s'accumulent — la dérive est régulière (loi des grands nombres)
    auto perturb = [&](float val) -> float {
        return glm::clamp(val + m_normal.sample(0.f, 0.05f), 0.f, 1.f);
    };

    m_currentLight = {perturb(m_currentLight.r), perturb(m_currentLight.g), perturb(m_currentLight.b)};
    m_currentDark  = {perturb(m_currentDark.r),  perturb(m_currentDark.g),  perturb(m_currentDark.b)};

    m_board.setChaosColors(
        {m_currentLight.r, m_currentLight.g, m_currentLight.b, 1.f},
        {m_currentDark.r,  m_currentDark.g,  m_currentDark.b,  1.f}
    );
    m_renderer.setChaosColors(m_currentLight, m_currentDark);
}

void Game::handlePostMove()
{
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
                m_renderer.orbit(m_cauchy.sample(0.f, 1.5f), m_cauchy.sample(0.f, 0.05f));
                perturbColors();
                m_chaosCountdown = m_geom.sample(0.4);
            }
        }
        m_winner = m_board.getWinner();
    }
    if (m_winner == PieceColor::None)
    {
        if (m_interface.getGameMode() == GameMode::OnePlayer)
        {
            // Différer : le joueur courant (humain) garde son éclairage pendant l'animation.
            // Le switch + coup IA s'exécuteront après animation+délai.
            m_pendingAIMove = AIPlayer::getMove(m_board, PieceColor::Black);
        }
        else
        {
            // Mode 2 joueurs : switch après animation+délai (éclairage cohérent)
            m_switchAfterAnim = true;
        }
    }
}

void Game::handle3DClick(int row, int col)
{
    auto [selRow, selCol] = m_board.getSelectedSquare();

    if (selRow >= 0)
    {
        // Vérifier si (row, col) est un coup valide
        auto validMoves = m_board.getValidMoves(selRow, selCol);
        for (auto [r, c] : validMoves)
        {
            if (r == row && c == col)
            {
                // Exécuter le coup
                TurnSnapshot preMove{m_board.takeSnapshot(), m_currentPlayer, m_moveHistory};
                m_undoStack.push_back(preMove);
                m_board.movePiece(selRow, selCol, row, col);
                m_board.setSelectedSquare(-1, -1);
                if (auto lm = m_board.getLastMove())
                {
                    m_moveHistory.push_back(formatMove(m_currentPlayer, *lm));
                    m_renderer.startPieceAnimation(lm->fromRow, lm->fromCol, lm->toRow, lm->toCol);
                    if (m_renderer.isPieceView())
                        m_renderer.updatePieceViewTarget(lm->toRow, lm->toCol);
                }
                handlePostMove();
                return;
            }
        }
    }

    // Sélectionner la pièce du joueur courant
    const Piece* p = m_board.getPiece(row, col);
    if (p && p->getColor() == m_currentPlayer)
        m_board.setSelectedSquare(row, col);
    else
        m_board.setSelectedSquare(-1, -1);
}

void Game::applyChaosEvent(std::string& logEntry)
{
    // X ~ Poisson(1.5) : nombre de pièces téléportées lors de cet événement
    int nbTeleports = m_poisson.sample(1.5);

    if (nbTeleports == 0)
    {
        logEntry = "[Chaos] Rien ne se passe...";
        return;
    }

    logEntry = "[Chaos] ";

    for (int t = 0; t < nbTeleports; ++t)
    {
        // Recalcul à chaque itération car le plateau a changé
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
            break;

        // X ~ U({0, ..., |pieces|-1}) : choisit la pièce à téléporter
        int pieceIdx            = m_uniform.sample(0, static_cast<int>(pieces.size()) - 1);
        auto [fromRow, fromCol] = pieces[pieceIdx];

        // Y ~ U({0, ..., |emptySquares|-1}) : choisit la case de destination
        int squareIdx       = m_uniform.sample(0, static_cast<int>(emptySquares.size()) - 1);
        auto [toRow, toCol] = emptySquares[squareIdx];

        const Piece* p = m_board.getPiece(fromRow, fromCol);

        const char* pieceName = "";
        switch (p->getType())
        {
        case PieceType::Pawn: pieceName = "Pion"; break;
        case PieceType::Rook: pieceName = "Tour"; break;
        case PieceType::Knight: pieceName = "Cavalier"; break;
        case PieceType::Bishop: pieceName = "Fou"; break;
        case PieceType::Queen: pieceName = "Dame"; break;
        default: pieceName = "Piece"; break;
        }
        const char* pieceColor = (p->getColor() == PieceColor::White) ? "B" : "N";

        if (t > 0)
            logEntry += ", ";
        logEntry += std::string(pieceColor) + "." + pieceName
                    + " " + (char)('a' + fromCol) + std::to_string(8 - fromRow)
                    + "->" + (char)('a' + toCol) + std::to_string(8 - toRow);

        m_board.movePiece(fromRow, fromCol, toRow, toCol);
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
    // Calcul du deltaTime (en secondes) entre deux frames
    auto  now = std::chrono::steady_clock::now();
    float dt  = std::chrono::duration<float>(now - m_lastFrameTime).count();
    if (dt > 0.5f) dt = 0.5f; // clamp au démarrage (premier frame)
    m_lastFrameTime = now;

    // Mode Infernal : setup binomial au premier frame (pions absents)
    if (m_interface.getGameMode() == GameMode::RandomMode && !m_chaosInitialized)
    {
        applyBinomialPawnSetup();
        // Premier délai spontané : X ~ Exp(0.2) → E[X] = 5 secondes
        m_spontaneousTimer = m_expDist.sample(1.f / 30.f);
        m_chaosInitialized = true;
    }

    // Mode Infernal : chaos spontané en temps réel cadencé par Exp(0.2)
    if (m_interface.getGameMode() == GameMode::RandomMode && m_chaosInitialized)
    {
        m_spontaneousTimer -= dt;
        if (m_spontaneousTimer <= 0.f)
        {
            std::string chaosLog;
            applyChaosEvent(chaosLog);
            if (!chaosLog.empty())
                m_moveHistory.push_back("[Spontané] " + chaosLog.substr(8));
            // Secousse caméra : Cauchy(0, 1.5) sur theta, Cauchy(0, 0.05) sur phi
            m_renderer.orbit(m_cauchy.sample(0.f, 1.5f), m_cauchy.sample(0.f, 0.05f));
            // Dérive de couleur : N(0, 0.05²) sur chaque composante RGB
            perturbColors();
            // Prochain chaos spontané : X ~ Exp(1/30)
            m_spontaneousTimer = m_expDist.sample(1.f / 30.f);
        }
    }

    // Toggle pause avec Echap
    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        m_paused = !m_paused;

    // Switch joueur différé (mode 2J) : après animation+délai
    if (m_switchAfterAnim && m_renderer.isReadyForNext())
    {
        m_switchAfterAnim = false;
        switchPlayer();
    }

    // Coup IA différé (mode 1J) : après animation humaine + délai
    if (m_pendingAIMove && m_renderer.isReadyForNext())
    {
        switchPlayer(); // humain → IA (éclairage bascule maintenant)
        auto ai = *m_pendingAIMove;
        m_pendingAIMove.reset();
        m_board.movePiece(ai.fromRow, ai.fromCol, ai.toRow, ai.toCol);
        if (auto lm = m_board.getLastMove())
        {
            m_moveHistory.push_back(formatMove(PieceColor::Black, *lm));
            m_renderer.startPieceAnimation(lm->fromRow, lm->fromCol, lm->toRow, lm->toCol);
        }
        m_winner = m_board.getWinner();
        if (m_winner == PieceColor::None)
            m_switchAfterAnim = true; // switch retour après animation IA + délai
    }

    m_renderer.setCurrentPlayer(m_currentPlayer);
    m_renderer.draw(m_board, dt);

    bool pauseRequested = false;
    bool undoRequested  = m_hud.draw(m_currentPlayer, m_moveHistory, !m_undoStack.empty(), pauseRequested);
    if (pauseRequested)
        m_paused = true;

    if (m_paused)
    {
        // Afficher le plateau 2D (non interactif) pour qu'il reste visible
        ImGui::BeginDisabled();
        m_board.drawBoard(m_currentPlayer);
        ImGui::EndDisabled();

        drawPauseMenu();

        ImGui::SetNextWindowSize(ImVec2(Renderer3D::FBO_WIDTH, Renderer3D::FBO_HEIGHT + 20), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Vue 3D"))
        {
            ImVec2 avail  = ImGui::GetContentRegionAvail();
            float  aspect = (float)Renderer3D::FBO_WIDTH / Renderer3D::FBO_HEIGHT;
            float  drawW  = avail.x;
            float  drawH  = drawW / aspect;
            if (drawH > avail.y) { drawH = avail.y; drawW = drawH * aspect; }
            float offX = (avail.x - drawW) * 0.5f;
            float offY = (avail.y - drawH) * 0.5f;
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offX, ImGui::GetCursorPosY() + offY));
            ImGui::Image((ImTextureID)(intptr_t)m_renderer.getTextureId(), ImVec2(drawW, drawH), ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();

        // Synchroniser sélection même en pause
        {
            auto [selR, selC] = m_board.getSelectedSquare();
            std::vector<std::pair<int,int>> validMoves;
            if (selR >= 0)
                validMoves = m_board.getValidMoves(selR, selC);
            m_renderer.setSelectionDisplay(selR, selC, std::move(validMoves));
        }

        if (m_winner != PieceColor::None)
            drawVictoryPopup();
        return;
    }

    // Sauvegarder l'état AVANT le coup (drawBoard applique le coup en interne)
    TurnSnapshot preMove{m_board.takeSnapshot(), m_currentPlayer, m_moveHistory};
    // Bloquer les coups pendant l'animation / le délai post-animation
    if (!m_renderer.isReadyForNext())
    {
        ImGui::BeginDisabled();
        m_board.drawBoard(m_currentPlayer);
        ImGui::EndDisabled();
    }
    else if (m_board.drawBoard(m_currentPlayer))
    {
        m_undoStack.push_back(preMove);
        if (auto lm = m_board.getLastMove())
        {
            m_moveHistory.push_back(formatMove(m_currentPlayer, *lm));
            m_renderer.startPieceAnimation(lm->fromRow, lm->fromCol, lm->toRow, lm->toCol);
            if (m_renderer.isPieceView())
                m_renderer.updatePieceViewTarget(lm->toRow, lm->toCol);
        }
        handlePostMove();
    }

    if (undoRequested && !m_undoStack.empty())
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
        float offX = (avail.x - drawW) * 0.5f;
        float offY = (avail.y - drawH) * 0.5f;
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offX, ImGui::GetCursorPosY() + offY));
        ImGui::Image(
            (ImTextureID)(intptr_t)m_renderer.getTextureId(),
            ImVec2(drawW, drawH),
            ImVec2(0, 1), ImVec2(1, 0)
        );

        // Touche P : bascule Trackball ↔ Vue Pièce
        if (ImGui::IsKeyPressed(ImGuiKey_P))
        {
            if (m_renderer.isPieceView())
            {
                m_renderer.enterTrackball();
            }
            else
            {
                auto [selRow, selCol] = m_board.getSelectedSquare();
                if (selRow != -1)
                {
                    const Piece* p = m_board.getPiece(selRow, selCol);
                    float pieceH = 0.45f;
                    if (p) {
                        switch (p->getType()) {
                        case PieceType::Pawn:   pieceH += 0.40f; break;
                        case PieceType::Rook:   pieceH += 0.50f; break;
                        case PieceType::Knight: pieceH += 0.55f; break;
                        case PieceType::Bishop: pieceH += 0.65f; break;
                        case PieceType::Queen:  pieceH += 0.80f; break;
                        case PieceType::King:   pieceH += 0.90f; break;
                        default: break;
                        }
                    }
                    float cx = selCol - 4.f + 0.5f;
                    float cz = selRow - 4.f + 0.5f;
                    m_renderer.enterPieceView(selRow, selCol, glm::vec3(cx, pieceH, cz));
                }
            }
        }

        if (ImGui::IsItemHovered())
        {
            const ImGuiIO& io  = ImGui::GetIO();
            ImVec2 imgMin      = ImGui::GetItemRectMin();
            float  relX        = io.MousePos.x - imgMin.x;
            float  relY        = io.MousePos.y - imgMin.y;
            bool   isDragging  = ImGui::IsMouseDragging(ImGuiMouseButton_Left)
                              || ImGui::IsMouseDragging(ImGuiMouseButton_Right);

            // Hover : mise à jour en continu (sans drag)
            if (!isDragging)
            {
                auto [hr, hc] = m_renderer.pickSquare(relX, relY, drawW, drawH);
                m_renderer.setHoverSquare(hr, hc);
            }
            else
            {
                m_renderer.setHoverSquare(-1, -1);
            }

            // Clic gauche sans drag → sélection / déplacement (bloqué pendant animation)
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && m_renderer.isReadyForNext())
            {
                auto [cr, cc] = m_renderer.pickSquare(relX, relY, drawW, drawH);
                if (cr >= 0)
                    handle3DClick(cr, cc);
            }

            // Drag gauche → rotation caméra
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                if (m_renderer.isPieceView())
                    m_renderer.lookAround(io.MouseDelta.x * 0.005f, -io.MouseDelta.y * 0.005f);
                else
                    m_renderer.orbit(-io.MouseDelta.x * 0.005f, io.MouseDelta.y * 0.005f);
            }

            if (!m_renderer.isPieceView())
            {
                if (io.MouseWheel != 0.0f)
                    m_renderer.zoom(io.MouseWheel * 1.5f);
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
                    m_renderer.pan(io.MouseDelta.x, io.MouseDelta.y);
            }
        }
        else
        {
            m_renderer.setHoverSquare(-1, -1);
        }

        // Synchroniser l'affichage de sélection avec l'état du plateau
        {
            auto [selR, selC] = m_board.getSelectedSquare();
            std::vector<std::pair<int,int>> validMoves;
            if (selR >= 0)
                validMoves = m_board.getValidMoves(selR, selC);
            m_renderer.setSelectionDisplay(selR, selC, std::move(validMoves));
        }

        // Indicateur de mode caméra
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offX + 6, ImGui::GetCursorPosY() - drawH + 6));
        ImGui::TextDisabled(m_renderer.isPieceView() ? "[P] Vue Piece  (P = retour)" : "[P] Trackball  (P = vue piece)");
    }
    ImGui::End();

    if (m_winner != PieceColor::None)
        drawVictoryPopup();
}