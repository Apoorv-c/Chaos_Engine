#pragma once
#include <glm/glm.hpp>

class Camera;

class Renderer {
public:
    static void Init();
    static Camera* GetCamera();
    static void Shutdown();

    static void BeginFrame();
    static void EndFrame();

    static void DrawTriangle(const glm::mat4& transform);
};




