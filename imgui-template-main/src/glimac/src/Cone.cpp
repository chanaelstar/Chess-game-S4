#include "glimac/Cone.hpp"
#include <cmath>
#include <iostream>
#include <vector>
#include "glimac/common.hpp"

namespace glimac {

void Cone::build(GLfloat height, GLfloat r, GLsizei discLat, GLsizei discHeight)
{
    GLfloat rcpLat = 1.f / discLat, rcpH = 1.f / discHeight;
    GLfloat dPhi = 2 * glm::pi<float>() * rcpLat, dH = height * rcpH;

    std::vector<ShapeVertex> data;

    // Construit l'ensemble des vertex
    for (GLsizei j = 0; j <= discHeight; ++j)
    {
        for (GLsizei i = 0; i < discLat; ++i)
        {
            ShapeVertex vertex;

            vertex.texCoords.x = i * rcpLat;
            vertex.texCoords.y = j * rcpH;

            vertex.position.x = r * (height - j * dH) * sin(i * dPhi) / height;
            vertex.position.y = j * dH;
            vertex.position.z = r * (height - j * dH) * cos(i * dPhi) / height;

            vertex.normal.x = sin(i * dPhi);
            vertex.normal.y = r / height;
            vertex.normal.z = cos(i * dPhi);
            vertex.normal   = glm::normalize(vertex.normal);

            data.push_back(vertex);
        }
    }

    m_nVertexCount = discLat * discHeight * 6;

    GLuint idx = 0;

    for (GLsizei j = 0; j < discHeight; ++j)
    {
        GLsizei offset = j * discLat;
        for (GLsizei i = 0; i < discLat; ++i)
        {
            m_Vertices.push_back(data[offset + i]);
            m_Vertices.push_back(data[offset + (i + 1) % discLat]);
            m_Vertices.push_back(data[offset + discLat + (i + 1) % discLat]);
            m_Vertices.push_back(data[offset + i]);
            m_Vertices.push_back(data[offset + discLat + (i + 1) % discLat]);
            m_Vertices.push_back(data[offset + i + discLat]);
        }
    }
}

} // namespace glimac
