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
        case Key::Q: return GLFW_KEY_Q;
        case Key::E: return GLFW_KEY_E;
        case Key::ESCAPE: return GLFW_KEY_ESCAPE;
        case Key::SPACE: return GLFW_KEY_SPACE;
        case Key::X: return GLFW_KEY_X;
        case Key::F5: return GLFW_KEY_F5;
        case Key::F9: return GLFW_KEY_F9;
        case Key::LEFT_ALT: return GLFW_KEY_LEFT_ALT;
        case Key::LEFT_CONTROL: return GLFW_KEY_LEFT_CONTROL;
        case Key::Z: return GLFW_KEY_Z;
        case Key::Y: return GLFW_KEY_Y;
        case Key::F: return GLFW_KEY_F;

    }
    return -1;
}

bool Input::IsKeyPressed(Key key) {
    auto window = g_MainWindow->GetNativeWindow();
    int state = glfwGetKey(window, ToGLFWKey(key));
    return state == GLFW_PRESS;
}
