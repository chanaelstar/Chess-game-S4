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

LoadedMesh loadOBJObject(const std::string& filepath, const std::string& objectName);
