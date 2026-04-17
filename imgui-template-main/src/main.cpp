#include <imgui.h>
#include "Game.hpp"
#include "quick_imgui/quick_imgui.hpp"

int main(int argc, char* argv[])
{
    Game game;

    quick_imgui::loop(
        "Chess",
        {.init = [&]() {
             ImGuiIO& io = ImGui::GetIO();
             io.Fonts->AddFontDefault();
             io.Fonts->AddFontFromFileTTF((std::string(CMAKE_SOURCE_DIR) + "/assets/fonts/MAGNFONT.TTF").c_str(), 60.0f);
             io.Fonts->Build();
             chessFont = io.Fonts->Fonts[1];

             game.init(); // init OpenGL
         },
         .loop                 = [&]() {
                game.update(); // ImGui + rendu 3D
                if (game.shouldQuit())
                    glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE); },
         .window_size_callback = [&](int width, int height) { game.onWindowResize(width, height); }}
    );
    return 0;
}