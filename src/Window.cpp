#include "Window.h"
#include <stdexcept>

#include <GLFW/glfw3.h>



constexpr int WINDOW_WIDTH = 1400;
constexpr int WINDOW_HEIGHT = 800;



Window::Window() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialise GLFW");
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "Revette",
        nullptr,
        nullptr
    );
    if (window == nullptr) {
        throw std::runtime_error("Failed to create window");
    }
}



Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}



GLFWwindow* Window::get() const {
    return window;
}
