#include "Core/Application.h"
#include "Core/Time.h"
#include "Core/Log.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene/Scene.h"
#include "Systems/SystemManager.h"
#include <cstdlib>
#include <string>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"



Window* g_MainWindow = nullptr;

Application::Application()
    : m_Window(1280, 720, "Chaos Engine") {
    g_MainWindow = &m_Window;
    // Create ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(g_MainWindow->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Log::Init();
    Log::Info("Engine starting...");
    Time::Init();

    m_Scene = new Scene();

    Renderer::Init();
}

void Application::Run() {
    while (!m_Window.ShouldClose()) {
        Time::Update();
        static float timer = 0.0f;
        static int frames = 0;

        timer += Time::DeltaTime();
        frames++;
        static bool spawnPressed = false;

        if (Input::IsKeyPressed(Key::SPACE)) {
            if (!spawnPressed) {
                spawnPressed = true;

                float x = ((rand() % 200) - 100) / 100.0f;
                float y = ((rand() % 200) - 100) / 100.0f;

                m_Scene->SpawnEntity({x, y, 0.0f});
            }
        }
        else {
            spawnPressed = false;
        }
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
        SystemManager::Update(*m_Scene, Time::DeltaTime());

        Renderer::BeginFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        SystemManager::Render(*m_Scene);
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
        static bool destroyPressed = false;

        if (Input::IsKeyPressed(Key::X)) {
            if (!destroyPressed) {
                destroyPressed = true;

                const auto& entities = m_Scene->GetEntities();
                if (!entities.empty()) {
                    Entity e = entities.back();
                    m_Scene->DestroyEntity(e);
                }
            }
        }
        else {
            destroyPressed = false;
        }
        // Save (Key: F5)
        static bool savePressed = false;
        if (Input::IsKeyPressed(Key::F5)) {
            if (!savePressed) {
                savePressed = true;
                m_Scene->Save("scene.txt");
            }
        }
        else {
            savePressed = false;
        }

        // Load (Key: F9)
        static bool loadPressed = false;
        if (Input::IsKeyPressed(Key::F9)) {
            if (!loadPressed) {
                loadPressed = true;
                m_Scene->Load("scene.txt");
            }
        }
        else {
            loadPressed = false;
        }
        ImGui::Begin("Chaos Engine");

        ImGui::Text("Entities: %d", (int)m_Scene->GetEntities().size());
        ImGui::Text("FPS: %.1f", 1.0f / Time::DeltaTime());

        if (ImGui::Button("Spawn"))
        {
            float x = ((rand() % 200) - 100) / 100.0f;
            float y = ((rand() % 200) - 100) / 100.0f;
            m_Scene->SpawnEntity({x, y, 0.0f});
        }

        if (ImGui::Button("Clear"))
        {
            m_Scene->Clear();
        }

        if (ImGui::Button("Save"))
        {
            m_Scene->Save("scene.txt");
        }

        if (ImGui::Button("Load"))
        {
            m_Scene->Load("scene.txt");
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        Renderer::EndFrame();
        if (timer >= 1.0f) {
            float fps = frames / timer;
            float frameTime = 1000.0f / fps;

            std::string title =
                "Chaos Engine | FPS: " + std::to_string((int)fps) +
                " | Frame: " + std::to_string(frameTime) + " ms" +
                " | Entities: " + std::to_string(m_Scene->GetEntities().size());

            glfwSetWindowTitle(g_MainWindow->GetNativeWindow(), title.c_str());

            frames = 0;
            timer = 0.0f;
        }

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
