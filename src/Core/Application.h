#pragma once
#include "Platform/Window.h"
extern class Window* g_MainWindow;
class Scene;


class Application {
public:
    Application();
    void Run();
    bool m_IsPlaying = false;
    Scene* m_RuntimeScene = nullptr;

private:
    Window m_Window;
    Scene* m_Scene;
};