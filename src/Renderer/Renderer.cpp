#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Time.h"
#include "Renderer/Camera.h"
#include "Input/Input.h"
#include <GLFW/glfw3.h>



static unsigned int VAO = 0;
static Shader* s_Shader = nullptr;
static Shader* s_PickingShader = nullptr;  // writes integer entity ID
static Camera* s_Camera = nullptr;
static unsigned int s_FBO = 0;
static unsigned int s_ColorTexture = 0;
static int s_FBWidth = 1280;
static int s_FBHeight = 720;
static unsigned int s_PickingFBO = 0;
static unsigned int s_PickingTexture = 0;


void Renderer::Init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.0f,  0.5f
    };

    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    const char* vs = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;
        void main() {
            gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 0.0, 1.0);
        }
    )";

    const char* fs = R"(
        #version 330 core
        out vec4 color;
        void main() {
            color = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )";

    s_Shader = new Shader(vs, fs);

    // Picking shader: same vertex transform, fragment writes integer entity ID.
    const char* pickVS = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;
        void main() {
            gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 0.0, 1.0);
        }
    )";
    const char* pickFS = R"(
        #version 330 core
        uniform int u_EntityID;
        out int fragColor;
        void main() {
            fragColor = u_EntityID;
        }
    )";
    s_PickingShader = new Shader(pickVS, pickFS);

    s_Camera = new Camera(-1.6f, 1.6f, -0.9f, 0.9f);

}


void Renderer::Shutdown() {
    // Placeholder for later
}

void Renderer::BeginFrame() {
    // Clear the default framebuffer so no stale geometry leaks through the dock.
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::PrepareShader() {
    s_Shader->Bind();
    s_Shader->SetMat4("u_ViewProjection", s_Camera->GetViewProjection());
}
void Renderer::DrawTriangle(const glm::mat4& transform) {
    s_Shader->SetMat4("u_Transform", transform);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
Camera* Renderer::GetCamera() {
    return s_Camera;
}
void Renderer::SetEntityID(int id)
{
    // Upload to the picking shader (which must already be bound via PreparePickingShader).
    s_PickingShader->SetInt("u_EntityID", id);
}

void Renderer::PreparePickingShader() {
    s_PickingShader->Bind();
    s_PickingShader->SetMat4("u_ViewProjection", s_Camera->GetViewProjection());
}

void Renderer::InitFramebuffer(int width, int height)
{
    s_FBWidth = width;
    s_FBHeight = height;

    // Generate FBO first, then bind and configure it.
    glGenFramebuffers(1, &s_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, s_FBO);

    glGenTextures(1, &s_ColorTexture);
    glBindTexture(GL_TEXTURE_2D, s_ColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_ColorTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer NOT complete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::ResizeFramebuffer(int width, int height)
{
    if (width == 0 || height == 0) return;
    if (width == s_FBWidth && height == s_FBHeight) return;

    s_FBWidth = width;
    s_FBHeight = height;

    // --- Resize color FBO texture and re-attach ---
    glBindTexture(GL_TEXTURE_2D, s_ColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, s_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_ColorTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // --- Resize picking FBO texture and re-attach ---
    glBindTexture(GL_TEXTURE_2D, s_PickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, s_PickingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_PickingTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::BindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, s_FBO);
    glViewport(0, 0, s_FBWidth, s_FBHeight);
}

void Renderer::UnbindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Restore viewport to the full window so ImGui renders correctly.
    GLFWwindow* win = glfwGetCurrentContext();
    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    glViewport(0, 0, w, h);
}

unsigned int Renderer::GetFramebufferTexture()
{
    return s_ColorTexture;
}
void Renderer::InitPickingFramebuffer(int width, int height)
{
    glGenFramebuffers(1, &s_PickingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, s_PickingFBO);

    glGenTextures(1, &s_PickingTexture);
    glBindTexture(GL_TEXTURE_2D, s_PickingTexture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R32I,
        width,
        height,
        0,
        GL_RED_INTEGER,
        GL_INT,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        s_PickingTexture,
        0
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("Picking framebuffer not complete\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::BindPickingFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, s_PickingFBO);
}
int Renderer::ReadEntityID(int x, int y)
{
    glBindFramebuffer(GL_FRAMEBUFFER, s_PickingFBO);

    int pixel = -1;

    glReadPixels(
        x,
        y,
        1,
        1,
        GL_RED_INTEGER,
        GL_INT,
        &pixel
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return pixel;
}
void Renderer::EndFrame() {
    // Nothing yet (swap handled by Window)
}
