#include "Core/Application.h"
#include "Core/Time.h"
#include "Core/Log.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene/Scene.h"


Window* g_MainWindow = nullptr;

Application::Application()
    : m_Window(1280, 720, "Chaos Engine") {
    g_MainWindow = &m_Window;
    Log::Init();
    Log::Info("Engine starting...");
    Time::Init();

    m_Scene = new Scene();

    Renderer::Init();
}

void Application::Run() {
    while (!m_Window.ShouldClose()) {
        Time::Update();
        Camera* cam = Renderer::GetCamera();
        static glm::vec3 camPos = {0.0f, 0.0f, 0.0f};
        float speed = 1.5f * Time::DeltaTime();
        if(Input::IsKeyPressed(Key::W)) {
            camPos.y += speed;
        }
        if(Input::IsKeyPressed(Key::S)) {
            camPos.y -= speed;
        }
        if(Input::IsKeyPressed(Key::A)) {
            camPos.x -= speed;
        }
        if(Input::IsKeyPressed(Key::D)) {
            camPos.x += speed;
        }
        cam->SetPosition(camPos);
        m_Scene->OnUpdate(Time::DeltaTime());
        Renderer::BeginFrame();
        m_Scene->OnRender();
        Renderer::EndFrame();

        static float angle = 0.0f;
        angle += Time::DeltaTime();

        glm::mat4 t1 = glm::mat4(1.0f);
        t1 = glm::rotate(t1, angle, glm::vec3(0, 0, 1));

        Renderer::DrawTriangle(t1);

        glm::mat4 t2 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.7f, 0.0f, 0.0f));
        Renderer::DrawTriangle(t2);

        glm::mat4 t3 = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.0f, 0.0f));
        Renderer::DrawTriangle(t3);

        static float zoom = 1.0f;

        if (Input::IsKeyPressed(Key::Q))
            zoom += Time::DeltaTime();

        if (Input::IsKeyPressed(Key::E))
            zoom -= Time::DeltaTime();

        Renderer::GetCamera()->SetProjection(
            -1.6f * zoom, 1.6f * zoom,
            -0.9f * zoom, 0.9f * zoom
        );

        


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
