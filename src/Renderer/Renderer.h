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
    static void PrepareShader();

    static void DrawTriangle(const glm::mat4& transform);
    static void InitFramebuffer(int width, int height);
    static void ResizeFramebuffer(int width, int height);
    static unsigned int GetFramebufferTexture();
    static void BindFramebuffer();
    static void UnbindFramebuffer();
};




