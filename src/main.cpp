#include <iostream>
#include <filesystem>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <render/Window.h>
#include "Game.h"

#ifdef _WIN32
#include "Windows.h"
#endif

// Force dedicated GPU (win only) (fixed bindless textures [probably laptop only issue?])
#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;  // for nvidia
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;    // for amd
}
#endif

static void setWorkingDirToExe(const char* argv0) {
#if defined(_WIN32)
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    std::filesystem::current_path(std::filesystem::path(exePath).parent_path());
#else
    if (argv0) {
        std::filesystem::current_path(
            std::filesystem::canonical(std::filesystem::path(argv0)).parent_path()
        );
    }
#endif
}

int gameEntry() {
    std::unique_ptr<engine::Window> window = std::make_unique<engine::Window>();
    if (!window->init(800, 600, "Testování voxelového enginu")) {
        std::cerr << "Failed to initialize window" << std::endl;
        return -1;
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int mx, my;
    glfwGetMonitorPos(monitor, &mx, &my);

    GLFWwindow* glfwWindow = window->window();
    glfwSwapInterval(0);

    Game game(std::move(window), glm::ivec2(mode->width, mode->height));

    glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_FALSE);
    glfwSetWindowPos(glfwWindow, mx, my);
    glfwSetWindowSize(glfwWindow, mode->width, mode->height);

    game.start();

    return 0;
}

#if defined(_WIN32)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    setWorkingDirToExe(nullptr);
    return gameEntry();
}

#endif

int main(int argc, char* argv[]) {
    setWorkingDirToExe(argc > 0 ? argv[0] : nullptr);
    return gameEntry();
}
