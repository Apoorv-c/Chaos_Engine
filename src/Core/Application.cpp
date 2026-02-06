#include "Core/Application.h"
#include "Core/Time.h"
#include "Core/Log.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"


Window* g_MainWindow = nullptr;

Application::Application()
    : m_Window(1280, 720, "Chaos Engine") {
    g_MainWindow = &m_Window;
    Log::Init();
    Log::Info("Engine starting...");
    Time::Init();

    Renderer::Init();
}

void Application::Run() {
    while (!m_Window.ShouldClose()) {
        Time::Update();
        Renderer::BeginFrame();
        Renderer::EndFrame();


        m_Window.Update();
        if(Input::IsKeyPressed(Key::ESCAPE)) {
            Log::Info("Escape key pressed - closing engine");
            break;
        }
    }
    Renderer::Shutdown();
    Log::Info("Engine shutting down...");
    Log::Shutdown();
}
