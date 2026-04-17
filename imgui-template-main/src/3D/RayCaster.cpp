#include "3D/RayCaster.hpp"
#include <cmath>
#include <glm/gtc/matrix_inverse.hpp>

std::pair<int, int> RayCaster::pickBoardSquare(
    glm::vec2 mouseRel, glm::vec2 imageSize,
    const glm::mat4& proj, const glm::mat4& view,
    float boardY
)
{
    float ndcX = 2.f * mouseRel.x / imageSize.x - 1.f;
    float ndcY = 1.f - 2.f * mouseRel.y / imageSize.y;

    glm::mat4 invProj = glm::inverse(proj);
    glm::vec4 rayEye  = invProj * glm::vec4(ndcX, ndcY, -1.f, 1.f);
    rayEye            = glm::vec4(rayEye.x, rayEye.y, -1.f, 0.f); // direction (w=0)

    glm::mat4 invView  = glm::inverse(view);
    glm::vec3 rayWorld = glm::normalize(glm::vec3(invView * rayEye));

    glm::vec3 rayOrigin = glm::vec3(invView * glm::vec4(0.f, 0.f, 0.f, 1.f));

    if (std::abs(rayWorld.y) < 1e-6f)
        return {-1, -1}; // rayon parallèle au plan

    float t = (boardY - rayOrigin.y) / rayWorld.y;
    if (t < 0.f)
        return {-1, -1}; // derrière la caméra

    glm::vec3 hit = rayOrigin + t * rayWorld;

    int col = static_cast<int>(std::floor(hit.x + 4.f));
    int row = static_cast<int>(std::floor(hit.z + 4.f));

    if (col < 0 || col > 7 || row < 0 || row > 7)
        return {-1, -1};

    return {row, col};
}
