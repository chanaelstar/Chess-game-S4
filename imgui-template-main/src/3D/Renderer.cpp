#include "3D/Renderer.hpp"
#include <cmath>
#include <glimac/FilePath.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "3D/ModelLoader.hpp"
#include "ChessBoard.hpp"


static void addBox(std::vector<float>& verts, float x, float z, float w, float d, float h)
{
    const float x1 = x, x2 = x + w;
    const float y1 = 0.f, y2 = h;
    const float z1 = z, z2 = z + d;

    auto addFace = [&](float ax, float ay, float az,
                       float bx, float by, float bz,
                       float cx, float cy, float cz,
                       float dx, float dy, float dz) {
        verts.insert(verts.end(), {ax, ay, az, bx, by, bz, cx, cy, cz, ax, ay, az, cx, cy, cz, dx, dy, dz});
    };

    addFace(x1, y2, z1, x2, y2, z1, x2, y2, z2, x1, y2, z2); // dessus
    addFace(x1, y1, z1, x2, y1, z1, x2, y2, z1, x1, y2, z1); // avant
    addFace(x1, y1, z2, x1, y2, z2, x2, y2, z2, x2, y1, z2); // arrière
    addFace(x1, y1, z1, x1, y2, z1, x1, y2, z2, x1, y1, z2); // gauche
    addFace(x2, y1, z1, x2, y1, z2, x2, y2, z2, x2, y2, z1); // droite
    addFace(x1, y1, z1, x1, y1, z2, x2, y1, z2, x2, y1, z1); // dessous
}

void Renderer3D::buildPieceMesh()
{
    std::vector<float> verts;
    const int          segments = 12;
    const float        twoPi    = glm::two_pi<float>();

    for (int i = 0; i < segments; ++i)
    {
        float a1 = twoPi * i / segments;
        float a2 = twoPi * (i + 1) / segments;
        float x1 = std::cos(a1), z1 = std::sin(a1);
        float x2 = std::cos(a2), z2 = std::sin(a2);

        // Fond
        verts.insert(verts.end(), {0, 0, 0, x1, 0, z1, x2, 0, z2});
        // Dessus
        verts.insert(verts.end(), {0, 1, 0, x2, 1, z2, x1, 1, z1});
        // Côté (quad)
        verts.insert(verts.end(), {x1, 0, z1, x2, 0, z2, x2, 1, z2, x1, 0, z1, x2, 1, z2, x1, 1, z1});
    }

    m_pieceVertexCount = (int)(verts.size() / 3);

    glGenVertexArrays(1, &m_pieceVao);
    glGenBuffers(1, &m_pieceVbo);
    glBindVertexArray(m_pieceVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_pieceVbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer3D::buildSkyboxMesh()
{
    // Cube unité [-1, 1] — les positions servent directement de directions
    static constexpr float verts[] = {
        // Arrière
        -1.f,
        1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        // Avant
        -1.f,
        -1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        // Gauche
        -1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        // Droite
        1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        // Bas
        -1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
        -1.f,
        -1.f,
        // Haut
        -1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        -1.f,
    };

    glGenVertexArrays(1, &m_skyboxVao);
    glGenBuffers(1, &m_skyboxVbo);
    glBindVertexArray(m_skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer3D::buildBorderMesh()
{
    std::vector<float> verts;

    const float h = 0.45f;
    const float t = 0.3f; // épaisseur du bord

    addBox(verts, -4.f - t, -4.f - t, 8.f + 2.f * t, t, h); // avant
    addBox(verts, -4.f - t, 4.f, 8.f + 2.f * t, t, h);      // arrière
    addBox(verts, -4.f - t, -4.f, t, 8.f, h);               // gauche
    addBox(verts, 4.f, -4.f, t, 8.f, h);                    // droite

    m_borderVertexCount = (int)(verts.size() / 3);

    glGenVertexArrays(1, &m_borderVao);
    glGenBuffers(1, &m_borderVbo);

    glBindVertexArray(m_borderVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_borderVbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer3D::buildBoardMesh()
{
    std::vector<float> vertices;

    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
            addBox(vertices, col - 4.f, row - 4.f, 1.f, 1.f, 0.45f);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer3D::recomputeViewMatrix()
{
    glm::vec3 eye;
    eye.x        = m_target.x + m_distance * std::cos(m_phi) * std::sin(m_theta);
    eye.y        = m_target.y + m_distance * std::sin(m_phi);
    eye.z        = m_target.z + m_distance * std::cos(m_phi) * std::cos(m_theta);
    m_viewMatrix = glm::lookAt(eye, m_target, glm::vec3(0.f, 1.f, 0.f));
}

void Renderer3D::orbit(float dTheta, float dPhi)
{
    m_theta += dTheta;
    const float TWO_PI = 2.0f * glm::pi<float>();
    if (m_theta > glm::pi<float>())
        m_theta -= TWO_PI;
    if (m_theta < -glm::pi<float>())
        m_theta += TWO_PI;
    m_phi = glm::clamp(m_phi + dPhi, glm::radians(5.f), glm::radians(89.f));
    recomputeViewMatrix();
}

void Renderer3D::zoom(float delta)
{
    m_distance = glm::clamp(m_distance - delta, 3.0f, 30.0f);
    recomputeViewMatrix();
}

void Renderer3D::setDistance(float d)
{
    m_distance = glm::clamp(d, 3.0f, 30.0f);
    recomputeViewMatrix();
}

void Renderer3D::setChaosColors(glm::vec3 light, glm::vec3 dark)
{
    m_colorLight = light;
    m_colorDark  = dark;
}

void Renderer3D::pan(float dx, float dy)
{
    glm::vec3 eye;
    eye.x             = m_target.x + m_distance * std::cos(m_phi) * std::sin(m_theta);
    eye.y             = m_target.y + m_distance * std::sin(m_phi);
    eye.z             = m_target.z + m_distance * std::cos(m_phi) * std::cos(m_theta);
    glm::vec3 forward = glm::normalize(m_target - eye);
    glm::vec3 right   = glm::normalize(glm::cross(forward, glm::vec3(0.f, 1.f, 0.f)));
    glm::vec3 up      = glm::normalize(glm::cross(right, forward));
    float     scale   = m_distance * 0.01f;
    m_target -= right * (dx * scale);
    m_target += up * (dy * scale);
    recomputeViewMatrix();
}

void Renderer3D::init()
{
    buildSkyboxMesh();
    buildBoardMesh();
    buildBorderMesh();
    buildPieceMesh();

    m_program = glimac::loadProgram(
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/board.vs.glsl",
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/board.fs.glsl"
    );
    m_program->use();
    m_uniMVP        = glGetUniformLocation(m_program->getGLId(), "uMVPMatrix");
    m_uniColor      = glGetUniformLocation(m_program->getGLId(), "uColor");
    m_uniTexture    = glGetUniformLocation(m_program->getGLId(), "uTexture");
    m_uniUseTexture = glGetUniformLocation(m_program->getGLId(), "uUseTexture");

    m_skyboxProgram = glimac::loadProgram(
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/skybox.vs.glsl",
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/skybox.fs.glsl"
    );
    m_skyboxUniVP = glGetUniformLocation(m_skyboxProgram->getGLId(), "uViewProj");

    // Shader pièces (normales + éclairage diffus)
    m_pieceProgram = glimac::loadProgram(
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/piece.vs.glsl",
        std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/piece.fs.glsl"
    );
    m_pieceProgram->use();
    m_pieceUniMVP   = glGetUniformLocation(m_pieceProgram->getGLId(), "uMVPMatrix");
    m_pieceUniModel = glGetUniformLocation(m_pieceProgram->getGLId(), "uModelMatrix");
    m_pieceUniColor = glGetUniformLocation(m_pieceProgram->getGLId(), "uColor");

    // Chargement des pièces depuis le fichier OBJ unique
    const std::string objFile = std::string(CMAKE_SOURCE_DIR) + "/assets/models/Chess_set_game.obj";
    m_pieceModels[static_cast<int>(PieceType::Pawn)]   = loadOBJObject(objFile, "Pedone1");
    m_pieceModels[static_cast<int>(PieceType::Rook)]   = loadOBJObject(objFile, "Torre1");
    m_pieceModels[static_cast<int>(PieceType::Knight)] = loadOBJObject(objFile, "Cavallo1");
    m_pieceModels[static_cast<int>(PieceType::Bishop)] = loadOBJObject(objFile, "Alfiere1");
    m_pieceModels[static_cast<int>(PieceType::Queen)]  = loadOBJObject(objFile, "Regina");
    m_pieceModels[static_cast<int>(PieceType::King)]   = loadOBJObject(objFile, "Re");

    glEnable(GL_DEPTH_TEST);

    m_projMatrix = glm::perspective(glm::radians(45.f), (float)FBO_WIDTH / FBO_HEIGHT, 0.1f, 100.f);
    recomputeViewMatrix();

    // Création du FBO
    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &m_fboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, FBO_WIDTH, FBO_HEIGHT);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer3D::draw(const ChessBoard& board)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, FBO_WIDTH, FBO_HEIGHT);

    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- Skybox ---
    // GL_LEQUAL obligatoire : la skybox force depth=1.0 via .xyww,
    // et le depth buffer est aussi à 1.0 après glClear → GL_LESS rejetterait tout.
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    m_skyboxProgram->use();
    glm::mat4 skyboxVP = m_projMatrix * glm::mat4(glm::mat3(m_viewMatrix));
    glUniformMatrix4fv(m_skyboxUniVP, 1, GL_FALSE, glm::value_ptr(skyboxVP));
    glBindVertexArray(m_skyboxVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS); // restaure le comportement normal

    // --- Plateau, bords et pièces ---
    m_program->use();

    glm::mat4 mvp = m_projMatrix * m_viewMatrix;
    glUniformMatrix4fv(m_uniMVP, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(m_uniUseTexture, 0); // Utiliser les couleurs, pas les textures

    // Draw border
    glm::vec3 borderColor = glm::vec3(0.35f, 0.20f, 0.08f);
    glUniform3fv(m_uniColor, 1, glm::value_ptr(borderColor));
    glBindVertexArray(m_borderVao);
    glDrawArrays(GL_TRIANGLES, 0, m_borderVertexCount);

    // Draw chess squares
    glBindVertexArray(m_vao);
    int vertexIndex = 0;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            bool      isLight = (row + col) % 2 == 0;
            glm::vec3 color   = isLight ? m_colorLight : m_colorDark;

            glUniform3fv(m_uniColor, 1, glm::value_ptr(color));

            glDrawArrays(GL_TRIANGLES, vertexIndex, 36);
            vertexIndex += 36;
        }
    }

    // Rendu des pièces avec les modèles OBJ
    auto targetHeight = [](PieceType type) -> float {
        switch (type)
        {
        case PieceType::Pawn:   return 0.40f;
        case PieceType::Rook:   return 0.50f;
        case PieceType::Knight: return 0.55f;
        case PieceType::Bishop: return 0.65f;
        case PieceType::Queen:  return 0.80f;
        case PieceType::King:   return 0.90f;
        default:                return 0.45f;
        }
    };

    if (m_pieceProgram)
    {
        m_pieceProgram->use();
        for (int row = 0; row < 8; ++row)
        {
            for (int col = 0; col < 8; ++col)
            {
                const Piece* piece = board.getPiece(row, col);
                if (!piece)
                    continue;

                const LoadedMesh& mesh = m_pieceModels[static_cast<int>(piece->getType())];
                if (!mesh.valid)
                    continue;

                float cx = col - 4.f + 0.5f;
                float cz = row - 4.f + 0.5f;

                // Centre du bounding box en XZ (les modèles ne sont pas centrés à l'origine)
                glm::vec3 bboxCenter = (mesh.bboxMin + mesh.bboxMax) * 0.5f;

                // Mise à l'échelle : cible la hauteur voulue
                float modelH = mesh.bboxMax.y - mesh.bboxMin.y;
                float scale  = (modelH > 0.f) ? targetHeight(piece->getType()) / modelH : 1.f;

                // Rotation spécifique par type (cavalier blanc face à l'adversaire)
                float extraRotY = 0.f;
                if (piece->getType() == PieceType::Knight && piece->getColor() == PieceColor::White)
                    extraRotY = glm::radians(180.f);

                // 1) Recentre le modèle (XZ) et pose la base à y=0
                // 2) Rotation éventuelle
                // 3) Met à l'échelle
                // 4) Translate vers la case cible (y=0.45 = dessus du plateau)
                glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(cx, 0.45f, cz))
                                  * glm::scale(glm::mat4(1.f), glm::vec3(scale))
                                  * glm::rotate(glm::mat4(1.f), extraRotY, glm::vec3(0.f, 1.f, 0.f))
                                  * glm::translate(glm::mat4(1.f), glm::vec3(-bboxCenter.x, -mesh.bboxMin.y, -bboxCenter.z));

                glm::vec3 color = (piece->getColor() == PieceColor::White)
                                      ? glm::vec3(0.95f, 0.93f, 0.85f)
                                      : glm::vec3(0.12f, 0.08f, 0.05f);

                glUniformMatrix4fv(m_pieceUniMVP,   1, GL_FALSE, glm::value_ptr(mvp * model));
                glUniformMatrix4fv(m_pieceUniModel, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3fv(m_pieceUniColor, 1, glm::value_ptr(color));

                glBindVertexArray(mesh.vao);
                glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
            }
        }
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
