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
    static void SetEntityID(int id);
    static void PreparePickingShader();
    static void DrawTriangle(
    const glm::mat4& transform,
    unsigned int textureID = 0
    );
    static void InitFramebuffer(int width, int height);
    static void ResizeFramebuffer(int width, int height);
    static unsigned int GetFramebufferTexture();
    static void BindFramebuffer();
    static void UnbindFramebuffer();
    static void InitPickingFramebuffer(int width, int height);
    static void BindPickingFramebuffer();
    static int ReadEntityID(int mouseX, int mouseY);
    static void DrawGrid();
    static void DrawPickingTriangle(const glm::mat4& transform);
    static void SetSelectedEntity(int entity);
    static int GetSelectedEntity();
};




