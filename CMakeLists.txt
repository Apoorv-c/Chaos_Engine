cmake_minimum_required(VERSION 3.20)
project(Chaos_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/glfw)
include_directories(external/stb)
add_executable(Chaos_Engine
    src/main.cpp
    src/Core/Application.cpp
    src/Platform/Window.cpp
    src/Core/Time.cpp
    src/Core/Log.cpp
    src/Input/Input.cpp
    src/Renderer/Renderer.cpp
    src/Renderer/Shader.cpp
    src/Renderer/Camera.cpp
    src/Scene/Scene.cpp
    src/Scene/Components.cpp
    src/Systems/RenderSystem.cpp
    src/Systems/TransformSystem.cpp
    src/Systems/SystemManager.cpp
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
    external/imguizmo/ImGuizmo.cpp
    src/Commands/CommandManager.cpp
    src/Commands/MoveCommand.cpp
    src/Commands/SpawnCommand.cpp
    src/Commands/DestroyCommand.cpp
    src/Renderer/Texture2D.cpp
    src/Tilemap/Tilemap.cpp
    src/Tilemap/TileRenderer.cpp
    src/Tilemap/TextureCache.cpp
    src/Tilemap/TileBrush.cpp


)
add_library(glad external/glad/src/glad.c)

target_include_directories(glad PUBLIC external/glad/include)

target_include_directories(Chaos_Engine PRIVATE
    src
    external/glfw/include
    external/glm
    external/imgui
    external/imgui/backends
    external/imguizmo
)

target_link_libraries(Chaos_Engine glfw glad)
