#include "3D/ModelLoader.hpp"
#include <cstdio>
#include <fstream>
#include <glm/glm.hpp>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

static void fanTriangulate(
    const std::vector<std::pair<int, int>>& poly, // (posIdx, normIdx) 0-based
    const std::vector<glm::vec3>&           positions,
    const std::vector<glm::vec3>&           normals,
    std::vector<float>&                     out,
    glm::vec3&                              bboxMin,
    glm::vec3&                              bboxMax
)
{
    for (int i = 1; i + 1 < static_cast<int>(poly.size()); ++i)
    {
        for (int k : {0, i, i + 1})
        {
            auto [pi, ni] = poly[k];
            glm::vec3 pos = positions[pi];
            out.insert(out.end(), {pos.x, pos.y, pos.z});
            bboxMin = glm::min(bboxMin, pos);
            bboxMax = glm::max(bboxMax, pos);

            glm::vec3 norm{0.f, 1.f, 0.f};
            if (ni >= 0 && ni < static_cast<int>(normals.size()))
                norm = normals[ni];
            out.insert(out.end(), {norm.x, norm.y, norm.z});
        }
    }
}

LoadedMesh loadOBJObject(const std::string& filepath, const std::string& objectName)
{
    std::ifstream file(filepath);
    if (!file.is_open())
        return {};

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<float>     verts;
    glm::vec3              bboxMin{std::numeric_limits<float>::max()};
    glm::vec3              bboxMax{std::numeric_limits<float>::lowest()};

    bool        inTarget = false;
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string        token;
        iss >> token;

        if (token == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            positions.push_back({x, y, z});
        }
        else if (token == "vn")
        {
            float x, y, z;
            iss >> x >> y >> z;
            normals.push_back({x, y, z});
        }
        else if (token == "o")
        {
            std::string name;
            iss >> name;
            inTarget = (name == objectName);
        }
        else if (token == "f" && inTarget)
        {
            std::vector<std::pair<int, int>> poly;
            std::string                      vtx;
            while (iss >> vtx)
            {
                int pi = 0, ti = 0, ni = -1;
                if (std::sscanf(vtx.c_str(), "%d/%d/%d", &pi, &ti, &ni) == 3)
                {
                }
                else if (std::sscanf(vtx.c_str(), "%d//%d", &pi, &ni) == 2)
                {
                }
                else if (std::sscanf(vtx.c_str(), "%d/%d", &pi, &ti) == 2)
                {
                    ni = -1;
                }
                else
                {
                    std::sscanf(vtx.c_str(), "%d", &pi);
                }
                poly.push_back({pi - 1, ni > 0 ? ni - 1 : -1});
            }
            fanTriangulate(poly, positions, normals, verts, bboxMin, bboxMax);
        }
    }

    if (verts.empty())
        return {};

    LoadedMesh mesh;
    mesh.vertexCount = static_cast<int>(verts.size() / 6);
    mesh.bboxMin     = bboxMin;
    mesh.bboxMax     = bboxMax;
    mesh.valid       = true;

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(verts.size() * sizeof(float)),
        verts.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // Normale : location 1 (3 floats)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    return mesh;
}
