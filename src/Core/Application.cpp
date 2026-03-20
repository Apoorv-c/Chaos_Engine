#include "Core/Application.h"
#include "Core/Time.h"
#include "Core/Log.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Scene/Scene.h"
#include "Systems/SystemManager.h"
#include <cstdlib>
#include <string>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imconfig.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include "ImGuizmo.h"


Window* g_MainWindow = nullptr;

Application::Application()
    : m_Window(1280, 720, "Chaos Engine") {
    g_MainWindow = &m_Window;
    // Create ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(g_MainWindow->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Log::Init();
    Log::Info("Engine starting...");
    Time::Init();

    m_Scene = new Scene();

    Renderer::Init();
    Renderer::InitPickingFramebuffer(1280, 720);
    Renderer::InitFramebuffer(1280, 720);
}

void Application::Run() {
    static int selectedEntity = -1;
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
        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport());
        
        ImGui::Begin("Scene");

        ImVec2 size = ImGui::GetContentRegionAvail();

        // Resize framebuffer to panel size
        Renderer::ResizeFramebuffer((int)size.x, (int)size.y);

        // --------------------
        // RENDER SCENE TO FBO
        // --------------------
        Renderer::BindFramebuffer();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Renderer::PrepareShader();
        Renderer::DrawGrid();
        // Grid uses its own shader; restore the entity shader before drawing entities.
        Renderer::PrepareShader();
        SystemManager::Render(*m_Scene);

        Renderer::UnbindFramebuffer();

        // --------------------
        // SHOW SCENE TEXTURE
        // --------------------
        ImGui::Image(
            (void*)(intptr_t)Renderer::GetFramebufferTexture(),
            size,
            ImVec2(0,1),
            ImVec2(1,0)
        );

        // --------------------
        // GIZMO SETUP
        // --------------------
        ImGuizmo::SetOrthographic(true);
        ImGuizmo::BeginFrame();

        // Scene viewport position
        ImVec2 imagePos  = ImGui::GetItemRectMin();
        ImVec2 imageSize = ImGui::GetItemRectSize();

        ImGuizmo::SetRect(
            imagePos.x,
            imagePos.y,
            imageSize.x,
            imageSize.y
        );

        // --------------------
        // DRAW GIZMO
        // --------------------
        // --------------------
        // DRAW GIZMO
        // --------------------
        if (selectedEntity >= 0)
        {
            auto& transform = m_Scene->GetTransform(selectedEntity);

            glm::mat4 transformMatrix = transform.GetMatrix();

            Camera* cam = Renderer::GetCamera();

            // Draw gizmo on the default ImGui drawlist
            ImGuizmo::SetDrawlist();

            // Use scene viewport rectangle
            ImVec2 imagePos  = ImGui::GetItemRectMin();
            ImVec2 imageSize = ImGui::GetItemRectSize();

            ImGuizmo::SetRect(
                imagePos.x,
                imagePos.y,
                imageSize.x,
                imageSize.y
            );

            // Build view matrix from camera position
            glm::mat4 view = glm::translate(glm::mat4(1.0f), -camPos);
            glm::mat4 projection = cam->GetProjection();

            ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                ImGuizmo::TRANSLATE,
                ImGuizmo::LOCAL,
                glm::value_ptr(transformMatrix)
            );

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;

                ImGuizmo::DecomposeMatrixToComponents(
                    glm::value_ptr(transformMatrix),
                    &translation.x,
                    &rotation.x,
                    &scale.x
                );

                transform.Position = translation;
            }
        }
        // --------------------
        // PICKING PASS
        // --------------------
        Renderer::BindPickingFramebuffer();

        glViewport(0,0,(int)size.x,(int)size.y);

        GLint clearVal = -1;
        glClearBufferiv(GL_COLOR,0,&clearVal);

        Renderer::PreparePickingShader();

        const auto& pickEntities = m_Scene->GetEntities();
        const auto& pickRenderables = m_Scene->GetRenderables();

        for (int i=0;i<(int)pickEntities.size();i++)
        {
            if(!pickRenderables[i].Visible) continue;

            Renderer::SetEntityID(i);
            Renderer::DrawTriangle(m_Scene->GetTransform(i).GetMatrix());
        }

        glBindFramebuffer(GL_FRAMEBUFFER,0);

        // Restore viewport
        GLFWwindow* win = glfwGetCurrentContext();
        int ww,wh;
        glfwGetFramebufferSize(win,&ww,&wh);
        glViewport(0,0,ww,wh);

        // --------------------
        // CLICK SELECT
        // --------------------
        if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
        {
            ImVec2 mouse = ImGui::GetMousePos();
            ImVec2 origin = ImGui::GetItemRectMin();

            int px = (int)(mouse.x-origin.x);
            int py = (int)(size.y-(mouse.y-origin.y));

            int id = Renderer::ReadEntityID(px,py);

            if(id>=0)
                selectedEntity = id;
        }

        ImGui::End();

        static float angle = 0.0f;
        angle += Time::DeltaTime();

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

        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("File")){
                if(ImGui::MenuItem("Save"))
                    m_Scene->Save("scene.txt");
                if(ImGui::MenuItem("Load"))
                    m_Scene->Load("scene.txt");
                if (ImGui::MenuItem("Exit"))
                    glfwSetWindowShouldClose(g_MainWindow->GetNativeWindow(), true);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

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

        // IMGUI WINDOW SPLITTING(HEIRARCHY , INSPECTOR , ENGINE)

        ImGui::Begin("Hierarchy");

        ImGui::Text("Entities");
        ImGui::Separator();

        const auto& entities = m_Scene->GetEntities();

        for (int i = 0; i < (int)entities.size(); i++)
        {
            if (!m_Scene->GetRender(i).Visible)
                continue;

            std::string label = "Entity " + std::to_string(i);

            if (ImGui::Selectable(label.c_str(), selectedEntity == i))
                selectedEntity = i;
        }

        ImGui::End();

        ImGui::Begin("Inspector");

        if (selectedEntity >= 0 && selectedEntity < (int)m_Scene->GetEntities().size())
        {
            auto& transform = m_Scene->GetTransform(selectedEntity);
            glm::mat4 transformMatrix = transform.GetMatrix();
            ImGui::Text("Transform");
            ImGui::DragFloat2("Position", &transform.Position.x, 0.01f);
            ImGui::DragFloat("Rotation", &transform.Rotation, 0.01f);

            ImGui::Separator();

            if (ImGui::Button("Delete"))
            {
                m_Scene->DestroyEntity(selectedEntity);
                selectedEntity = -1;
            }
        }

        ImGui::End();

        ImGui::Begin("Engine");

        ImGui::Text("FPS: %.1f", 1.0f / Time::DeltaTime());
        ImGui::Text("Entities: %d", (int)m_Scene->GetEntities().size());

        ImGui::Separator();

        // Spawn
        if (ImGui::Button("Spawn"))
        {
            float x = ((rand() % 200) - 100) / 100.0f;
            float y = ((rand() % 200) - 100) / 100.0f;
            m_Scene->SpawnEntity({x, y, 0.0f});
        }

        // Save / Load
        if (ImGui::Button("Save"))
            m_Scene->Save("scene.txt");

        if (ImGui::Button("Load"))
            m_Scene->Load("scene.txt");

        // Camera controls
        ImGui::Separator();
        ImGui::Text("Camera");

        auto* camera = Renderer::GetCamera();
        ImGui::DragFloat2("Cam Pos", &camPos.x, 0.05f);
        camera->SetPosition(camPos);

        ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.2f, 5.0f);
        camera->SetProjection(
            -1.6f * zoom, 1.6f * zoom,
            -0.9f * zoom, 0.9f * zoom
        );

        // Wireframe
        static bool wireframe = false;
        if (ImGui::Checkbox("Wireframe", &wireframe))
        {
            if (wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // VSync
        static bool vsync = true;
        if (ImGui::Checkbox("VSync", &vsync))
            glfwSwapInterval(vsync ? 1 : 0);

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
