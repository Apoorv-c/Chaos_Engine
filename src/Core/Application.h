#pragma once
#include "Platform/Window.h"
extern class Window* g_MainWindow;
class Scene;


class Application {
public:
    Application();
    void Run();

private:
    Window m_Window;
    Scene* m_Scene;
};