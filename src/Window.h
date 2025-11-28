#pragma once



class Window {
private:
    struct GLFWwindow* window;

public:
    Window();
    ~Window();

    Window(Window&&) = delete;
    Window(const Window&) = delete;
    Window operator=(Window&&) = delete;
    Window operator=(const Window&) = delete;

    struct GLFWwindow* get() const;
};
