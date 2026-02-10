cmake_minimum_required(VERSION 3.20)
project(Chaos_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/glfw)

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



)
add_library(glad external/glad/src/glad.c)

target_include_directories(glad PUBLIC external/glad/include)

target_include_directories(Chaos_Engine PRIVATE
    src
    external/glfw/include
    external/glm

)

target_link_libraries(Chaos_Engine glfw glad)
