#include "Input/Input.h"
#include "Platform/Window.h"
#include "Core/Application.h"
#include <GLFW/glfw3.h>

// Temporary global access (we’ll clean this later)
extern Window* g_MainWindow;

static int ToGLFWKey(Key key) {
    switch (key) {
        case Key::W: return GLFW_KEY_W;
        case Key::A: return GLFW_KEY_A;
        case Key::S: return GLFW_KEY_S;
        case Key::D: return GLFW_KEY_D;
        case Key::ESCAPE: return GLFW_KEY_ESCAPE;
        case Key::SPACE: return GLFW_KEY_SPACE;
    }
    return -1;
}

bool Input::IsKeyPressed(Key key) {
    auto window = g_MainWindow->GetNativeWindow();
    int state = glfwGetKey(window, ToGLFWKey(key));
    return state == GLFW_PRESS;
}
