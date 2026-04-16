#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

struct LoadedMesh {
    GLuint    vao{};
    GLuint    vbo{};
    int       vertexCount{};
    glm::vec3 bboxMin{};
    glm::vec3 bboxMax{};
    bool      valid{false};
};

// Charge un objet nommé depuis un fichier OBJ multi-objets
LoadedMesh loadOBJObject(const std::string& filepath, const std::string& objectName);
