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
#include "Commands/CommandManager.h"
#include "Commands/MoveCommand.h"
#include "Commands/SpawnCommand.h"
#include "Commands/DestroyCommand.h"
#include <memory>
#include <filesystem>
#include "Renderer/Texture2D.h"


Window* g_MainWindow = nullptr;
static float g_ScrollOffset = 0.0f;

void ScrollCallback(
    GLFWwindow* window,
    double xOffset,
    double yOffset)
{
    g_ScrollOffset =
        (float)yOffset;
}

Application::Application()
    : m_Window(1280, 720, "Chaos Engine") {
    g_MainWindow = &m_Window;
    // Create ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(g_MainWindow->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Register scroll callback AFTER ImGui init so it doesn't get overwritten
    glfwSetScrollCallback(
        g_MainWindow->GetNativeWindow(),
        ScrollCallback
    );

    Log::Init();
    Log::Info("Engine starting...");
    Time::Init();

    m_Scene = new Scene();

    Renderer::Init();
    Renderer::InitPickingFramebuffer(1280, 720);
    Renderer::InitFramebuffer(1280, 720);
}

void Application::Run() {
    static double lastMouseX = 0.0;
    static double lastMouseY = 0.0;
    static bool firstMouse = true;
    static bool sceneHovered = false;   // updated each frame from previous frame's gizmo state
    static bool gizmoInUse  = false;    // previous-frame gizmo state
    static int selectedEntity = -1;
    static Texture2D folderIcon(
        "D:/Chaos_Engine/Assets/Editor/folder.png");

    static Texture2D fileIcon(
        "D:/Chaos_Engine/Assets/Editor/file.png");

    static Texture2D imageIcon(
        "D:/Chaos_Engine/Assets/Editor/image.png");
    static std::filesystem::path currentDirectory = "D:/Chaos_Engine/Assets";
    while (!m_Window.ShouldClose()) {
        Time::Update();

        // ── Mouse delta (must be inside the loop) ────────────────────────────
        double mouseX, mouseY;
        glfwGetCursorPos(g_MainWindow->GetNativeWindow(), &mouseX, &mouseY);
        if (firstMouse) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouse = false;
        }
        double deltaX = mouseX - lastMouseX;
        double deltaY = mouseY - lastMouseY;
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        // ─────────────────────────────────────────────────────────────────────

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

                CommandManager::ExecuteCommand(
                    std::make_unique<SpawnCommand>(m_Scene, glm::vec3{x, y, 0.0f})
                );
            }
        }
        else {
            spawnPressed = false;
        }

        Camera* cam = Renderer::GetCamera();
        static glm::vec3 camPos = {0.0f, 0.0f, 0.0f};
        static float zoom = 1.0f;
        static float targetZoom = 1.0f;
        static float scrollDelta = 0.0f;
        float gridSize = 1.0f;
        static ImVec2 lastMousePos = {0,0};
        static glm::vec3 targetCamPos = camPos;

        bool altPressed  = Input::IsKeyPressed(Key::LEFT_ALT);
        bool leftMouse   = glfwGetMouseButton(g_MainWindow->GetNativeWindow(), GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS;
        bool rightMouse  = glfwGetMouseButton(g_MainWindow->GetNativeWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

        // WASD only when scene is hovered and ALT is NOT held
        if (sceneHovered && !altPressed) {
            float speed = 1.5f * Time::DeltaTime();
            if (Input::IsKeyPressed(Key::W)) targetCamPos.y += speed;
            if (Input::IsKeyPressed(Key::S)) targetCamPos.y -= speed;
            if (Input::IsKeyPressed(Key::A)) targetCamPos.x -= speed;
            if (Input::IsKeyPressed(Key::D)) targetCamPos.x += speed;
        }
        Scene* activeScene =  m_IsPlaying
            ? m_RuntimeScene
            : m_Scene;

        if (Input::IsKeyPressed(Key::F))
        {
            if (selectedEntity >= 0)
            {
                targetCamPos =
                    activeScene
                    ->GetTransform(selectedEntity)
                    .Position;
            }
        }
        bool alt =
            Input::IsKeyPressed(Key::LEFT_ALT);

        ImVec2 currentMousePos = {(float)mouseX, (float)mouseY};

        if (alt && ImGui::IsMouseDragging(2))
        {
            ImVec2 delta =
            {
                currentMousePos.x - lastMousePos.x,
                currentMousePos.y - lastMousePos.y
            };

            targetCamPos.x -=
                delta.x
                * zoom
                * 0.0015f;

            targetCamPos.y +=
                delta.y
                * zoom
                * 0.0015f;
        }

        lastMousePos = currentMousePos;
        // Undo / Redo  —  one-shot (no repeat every frame while key held)
        static bool undoPressed = false;
        static bool redoPressed = false;
        bool ctrl = Input::IsKeyPressed(Key::LEFT_CONTROL);
        bool zKey = Input::IsKeyPressed(Key::Z);
        bool yKey = Input::IsKeyPressed(Key::Y);

        if (ctrl && zKey) { if (!undoPressed) { CommandManager::Undo(); undoPressed = true; } }
        else              { undoPressed = false; }

        if (ctrl && yKey) { if (!redoPressed) { CommandManager::Redo(); redoPressed = true; } }
        else              { redoPressed = false; }

        // ALT + LMB → smooth pan   |   ALT + RMB → smooth zoom
        if (sceneHovered && altPressed && !gizmoInUse)
        {
            if (leftMouse)
            {
                // Pan: drag right → camera moves right (no vanish)
                float panSpeed = 0.005f * zoom;
                camPos.x -= (float)deltaX * panSpeed;
                camPos.y += (float)deltaY * panSpeed;
            }

            if (rightMouse)
            {
                // Zoom: drag RIGHT = zoom in, drag LEFT = zoom out (no reverse)
                targetZoom *= (1.0f - (float)deltaX * 0.01f);
            }
        }
        camPos +=
            (targetCamPos - camPos)
            * 8.0f
            * Time::DeltaTime();
        cam->SetPosition(camPos);

        Renderer::BeginFrame();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImVec2 mousePos =
            ImGui::GetMousePos();
        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport());
        
        ImGui::Begin("Scene");
        ImVec2 size = ImGui::GetContentRegionAvail();
        // Update sceneHovered AFTER rendering the gizmo (see end of gizmo block).
        // Here we only update the window-level hover; gizmo exclusion uses previous frame.
        bool sceneWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        sceneHovered = sceneWindowHovered && !gizmoInUse;
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
        SystemManager::Render(*activeScene);

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

        // Drop target on the scene viewport image
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("ASSET_PATH"))
            {
                const char* droppedPath =
                    (const char*)payload->Data;

                if (selectedEntity >= 0)
                {
                    activeScene->GetTexture(selectedEntity).Path =
                        droppedPath;
                }
            }

            ImGui::EndDragDropTarget();
        }

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
        // ── oldPosition tracked per drag, reset on selection change ─────────
        static glm::vec3 oldPosition = {};
        static int        lastSelected = -1;

        if (selectedEntity >= 0)
        {
            auto& transform = activeScene->GetTransform(selectedEntity);
            glm::mat4 transformMatrix = transform.GetMatrix();
            Camera* cam = Renderer::GetCamera();

            // Reset oldPosition when selection changes
            if (selectedEntity != lastSelected)
            {
                oldPosition  = transform.Position;
                lastSelected = selectedEntity;
            }

            ImGuizmo::SetDrawlist();

            ImVec2 imagePos  = ImGui::GetItemRectMin();
            ImVec2 imageSize = ImGui::GetItemRectSize();
            ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

            glm::mat4 view       = glm::translate(glm::mat4(1.0f), -camPos);
            glm::mat4 projection = cam->GetProjection();

            bool wasUsing = gizmoInUse;

            ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                ImGuizmo::TRANSLATE,
                ImGuizmo::LOCAL,
                glm::value_ptr(transformMatrix)
            );

            bool isUsing = ImGuizmo::IsUsing();

            if (isUsing)
            {
                glm::vec3 translation, rotation, scale;
                ImGuizmo::DecomposeMatrixToComponents(
                    glm::value_ptr(transformMatrix),
                    &translation.x, &rotation.x, &scale.x
                );

                // Capture start position on first frame of drag
                if (!wasUsing)
                    oldPosition = transform.Position;

                transform.Position = translation;
            }
            else if (wasUsing)
            {
                // Gizmo just released → record a single undo command
                glm::vec3 newPos = transform.Position;
                if (oldPosition != newPos)
                {
                    CommandManager::ExecuteCommand(
                        std::make_unique<MoveCommand>(
                            activeScene, selectedEntity, oldPosition, newPos
                        )
                    );
                }
                oldPosition = newPos;
            }

            gizmoInUse = isUsing;
        }
        else
        {
            gizmoInUse   = false;
            lastSelected = -1;
        }

        // --------------------
        // PICKING PASS
        // --------------------
        Renderer::BindPickingFramebuffer();

        glViewport(0, 0, (int)size.x, (int)size.y);

        GLint clearVal = -1;
        glClearBufferiv(GL_COLOR, 0, &clearVal);

        Renderer::PreparePickingShader();

        const auto& pickEntities    = activeScene->GetEntities();
        const auto& pickRenderables = activeScene->GetRenderables();

        for (int i = 0; i < (int)pickEntities.size(); i++)
        {
            if (!pickRenderables[i].Visible) continue;
            Renderer::SetEntityID(i);
            Renderer::DrawPickingTriangle(activeScene->GetTransform(i).GetMatrix());
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Restore viewport
        GLFWwindow* win = glfwGetCurrentContext();
        int ww, wh;
        glfwGetFramebufferSize(win, &ww, &wh);
        glViewport(0, 0, ww, wh);

        // --------------------
        // CLICK SELECT
        // --------------------
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0) && !altPressed && !gizmoInUse)
        {
            ImVec2 mouse  = ImGui::GetMousePos();
            ImVec2 origin = ImGui::GetItemRectMin();

            int px = (int)(mouse.x - origin.x);
            int py = (int)(size.y  - (mouse.y - origin.y));

            int id = Renderer::ReadEntityID(px, py);
            if (id >= 0)
            {
                selectedEntity = id;
                Renderer::SetSelectedEntity(id);
                oldPosition = activeScene->GetTransform(id).Position; // reset undo baseline
            }
        }

        ImGui::End();

        static float angle = 0.0f;
        angle += Time::DeltaTime();

        
        targetZoom -= g_ScrollOffset * 0.2f;
        g_ScrollOffset = 0.0f;

        // Clamp BEFORE interpolation so zoom never goes out of range
        if (targetZoom < 0.2f) targetZoom = 0.2f;
        if (targetZoom > 5.0f) targetZoom = 5.0f;

        zoom += (targetZoom - zoom) * 8.0f * Time::DeltaTime();
        if (zoom < 0.5f)
            gridSize = 0.25f;
        else if (zoom < 1.5f)
            gridSize = 0.5f;
        else if (zoom < 3.0f)
            gridSize = 1.0f;
        else
            gridSize = 2.0f;
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

                const auto& entities = activeScene->GetEntities();
                int lastVisible = -1;
                for (int i = (int)entities.size() - 1; i >= 0; i--) {
                    if (activeScene->GetRender(i).Visible) {
                        lastVisible = i;
                        break;
                    }
                }
                if (lastVisible >= 0) {
                    CommandManager::ExecuteCommand(
                        std::make_unique<DestroyCommand>(activeScene, lastVisible)
                    );
                    if (selectedEntity == lastVisible) {
                        selectedEntity = -1;
                    }
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

        const auto& entities = activeScene->GetEntities();

        for (int i = 0; i < (int)entities.size(); i++)
        {
            if (!activeScene->GetRender(i).Visible)
                continue;

            std::string label = "Entity " + std::to_string(i);

            if (ImGui::Selectable(label.c_str(), selectedEntity == i))
                selectedEntity = i;
        }

        ImGui::End();

        ImGui::Begin("Inspector");

        if (selectedEntity >= 0 && selectedEntity < (int)activeScene->GetEntities().size())
        {
            auto& transform = activeScene->GetTransform(selectedEntity);
            glm::mat4 transformMatrix = transform.GetMatrix();
            ImGui::Text("Transform");
            ImGui::DragFloat2("Position", &transform.Position.x, 0.01f);
            ImGui::DragFloat("Rotation", &transform.Rotation, 0.01f);

            
            auto& texture =
                activeScene->GetTexture(selectedEntity);

            ImGui::Separator();

            ImGui::Text("Texture");

            ImGui::TextWrapped(
                "%s",
                texture.Path.empty() ? "(none - drag a texture here)" : texture.Path.c_str()
            );

            // Drop target on the texture label in Inspector
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload("ASSET_PATH"))
                {
                    const char* droppedPath =
                        (const char*)payload->Data;
                    texture.Path = droppedPath;
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::Separator();

            if (ImGui::Button("Delete"))
            {
                CommandManager::ExecuteCommand(
                    std::make_unique<DestroyCommand>(activeScene, selectedEntity)
                );
                selectedEntity = -1;
            }
        }

        ImGui::End();

        ImGui::Begin("Assets");
        ImGui::Text("Current Folder:");
        ImGui::Text("%s", 
            currentDirectory.string().c_str());

        ImGui::Separator();
        if (currentDirectory != "Assets")

        {
        if (ImGui::Button("<- Back"))
        {
            currentDirectory =
                currentDirectory.parent_path();
        }
    }
        try {
        for (const auto& entry :
            std::filesystem::directory_iterator(currentDirectory))
        {
            std::string name =
                entry.path().filename().string();

            if (entry.is_directory())
            {
                ImGui::Image(
                    (void*)(intptr_t)folderIcon.GetID(),
                    ImVec2(16,16));

                ImGui::SameLine();

                if (ImGui::Selectable(name.c_str()))
                {
                    currentDirectory = entry.path();
                }
            }
            else
            {
                Texture2D* icon = &fileIcon;

                std::string ext =
                    entry.path().extension().string();

                if (ext == ".png" ||
                    ext == ".jpg")
                {
                    icon = &imageIcon;
                }

                ImGui::Image(
                    (void*)(intptr_t)icon->GetID(),
                    ImVec2(16,16));

                ImGui::SameLine();

                ImGui::Selectable(name.c_str());

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    std::string pathStr =
                        entry.path().string();

                    ImGui::SetDragDropPayload(
                        "ASSET_PATH",
                        pathStr.c_str(),
                        pathStr.size() + 1
                    );

                    ImGui::Text("%s", name.c_str());

                    ImGui::EndDragDropSource();
                }
            }
        }
        } catch (const std::filesystem::filesystem_error&) {
            ImGui::Text("Error reading directory");
        }

        ImGui::End();

        ImGui::Begin("Engine");

        ImGui::Text("FPS: %.1f", 1.0f / Time::DeltaTime());
        ImGui::Text("Entities: %d", (int)activeScene->GetEntities().size());

        ImGui::Separator();

        // Spawn
        if (ImGui::Button("Spawn"))
        {
            float x = ((rand() % 200) - 100) / 100.0f;
            float y = ((rand() % 200) - 100) / 100.0f;
            CommandManager::ExecuteCommand(
                std::make_unique<SpawnCommand>(activeScene, glm::vec3{x, y, 0.0f})
            );
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

        ImGui::Separator();

        if (!m_IsPlaying)
        {
            if (ImGui::Button("Play"))
            {
                m_IsPlaying = true;
                m_RuntimeScene = new Scene(*m_Scene);
            }
        }
        else
        {
            if (ImGui::Button("Stop"))
            {
                m_IsPlaying = false;
                selectedEntity = -1;
                delete m_RuntimeScene;
                m_RuntimeScene = nullptr;
            }
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
                " | Entities: " + std::to_string(activeScene->GetEntities().size());

            glfwSetWindowTitle(g_MainWindow->GetNativeWindow(), title.c_str());

            frames = 0;
            timer = 0.0f;
        }

        lastMousePos = mousePos;
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
