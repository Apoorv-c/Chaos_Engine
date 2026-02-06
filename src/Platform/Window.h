#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    void Update();
    bool ShouldClose() const;
    GLFWwindow* GetNativeWindow() const { return m_Window; }

private:
    GLFWwindow* m_Window;
};
