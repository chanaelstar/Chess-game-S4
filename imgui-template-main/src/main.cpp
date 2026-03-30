#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glimac/Program.hpp>
#include <glimac/Sphere.hpp>
#include <iostream>
#include "Game.hpp"
#include "quick_imgui/quick_imgui.hpp"

int window_width  = 800;
int window_height = 800;

int main(int argc, char* argv[])
{
    Game  game;
    float value{0.f};

    glimac::Sphere sphere(1, 32, 16);
    GLuint         vao;
    GLuint         vbo;

    glimac::FilePath               applicationPath((argv[0]));
    std::optional<glimac::Program> program;

    glm::mat4 ProjMatrix;
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;

    GLint uniMVP;
    GLint uniMV;
    GLint uniNormal;

    quick_imgui::loop(
        "Chess",
        {
            .init = [&]() {
             ImGuiIO& io = ImGui::GetIO();
             io.Fonts->AddFontDefault();
             io.Fonts->AddFontFromFileTTF((std::string(CMAKE_SOURCE_DIR) + "/assets/fonts/MAGNFONT.TTF").c_str(), 60.0f);
             io.Fonts->Build();
             chessFont = io.Fonts->Fonts[1]; // Get the chess font we just loaded

             // OPENGL

             // Création du VBO
             glGenBuffers(1, &vbo);
             glBindBuffer(GL_ARRAY_BUFFER, vbo);
             glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(glimac::ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
             glBindBuffer(GL_ARRAY_BUFFER, 0);

             // Création du VAO
             glGenVertexArrays(1, &vao);
             glBindVertexArray(vao);
             glBindBuffer(GL_ARRAY_BUFFER, vbo);

             glEnableVertexAttribArray(0);
             glVertexAttribPointer(
                 0, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex),
                 (const GLvoid*)offsetof(glimac::ShapeVertex, position)
             );

             glEnableVertexAttribArray(1);
             glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));

             glEnableVertexAttribArray(2);
             glVertexAttribPointer(
                 2, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex),
                 (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords)
             );

             glBindBuffer(GL_ARRAY_BUFFER, 0);
             glBindVertexArray(0);

              program = glimac::loadProgram(
                 std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/3D.vs.glsl",
                 std::string(CMAKE_SOURCE_DIR) + "/assets/shaders/normals.fs.glsl"
             );
             program->use();

             // Récupération des locations des variables uniformes
              uniMVP    = glGetUniformLocation(program->getGLId(), "uMVPMatrix");
              uniMV     = glGetUniformLocation(program->getGLId(), "uMVMatrix");
              uniNormal = glGetUniformLocation(program->getGLId(), "uNormalMatrix");

             glEnable(GL_DEPTH_TEST);

             ProjMatrix = glm::perspective(
                 glm::radians(70.f), (float)window_width / window_height, 0.1f, 100.f
             );

             MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));

             NormalMatrix = glm::transpose(glm::inverse(MVMatrix)); },

            .loop                 = [&]() {
             ImGui::ShowDemoWindow(); // This opens a window which shows tons of examples of what you can do with ImGui. You should check it out! Also, you can use the "Item Picker" in the top menu of that demo window: then click on any widget and it will show you the corresponding code directly in your IDE!

             ImGui::Begin("Example");

             ImGui::SliderFloat("My Value", &value, 0.f, 3.f);

             if (ImGui::Button("1", ImVec2{50.f, 50.f}))
                 std::cout << "Clicked button 1\n";
             ImGui::SameLine(); // Draw the next ImGui widget on the same line as the previous one. Otherwise it would be below it

             ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{1.f, 0.f, 0.f, 1.f}); // Changes the color of all buttons until we call ImGui::PopStyleColor(). There is also ImGuiCol_ButtonActive and ImGuiCol_ButtonHovered

             ImGui::PushID(2); // When some ImGui items have the same label (for exemple the next two buttons are labeled "Yo") ImGui needs you to specify an ID so that it can distinguish them. It can be an int, a pointer, a string, etc.
                               // You will definitely run into this when you create a button for each of your chess pieces, so remember to give them an ID!
             if (ImGui::Button("Yo", ImVec2{50.f, 50.f}))
                 std::cout << "Clicked button 2\n";
             ImGui::PopID(); // Then pop the id you pushed after you created the widget

             ImGui::SameLine();
             ImGui::PushID(3);
             if (ImGui::Button("Yo", ImVec2{50.f, 50.f}))
                 std::cout << "Clicked button 3\n";
             ImGui::PopID();

             ImGui::PopStyleColor();

             ImGui::End();
             game.update();
             if (game.shouldQuit())
                 glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);

             // OPENGL
             glClearColor(1.f, 0.5f, 0.5f, 1.f);
             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
             /*********************************
              * HERE SHOULD COME THE RENDERING CODE
              *********************************/

             glm::mat4 MVPMatrix = ProjMatrix * MVMatrix;

             glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
             glUniformMatrix4fv(uniMV, 1, GL_FALSE, glm::value_ptr(MVMatrix));
             glUniformMatrix4fv(uniNormal, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

             glBindVertexArray(vao);

             glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());

             glBindVertexArray(0); },
            .window_size_callback = [&](int width, int height) {
                window_width  = width;
                window_height = height;
                 glViewport(0, 0, width, height);
                 ProjMatrix = glm::perspective(glm::radians(70.f), (float)width / height, 0.1f, 100.f); },
        }
    );

    return 0;
}