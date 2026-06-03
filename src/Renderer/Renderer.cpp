#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Time.h"
#include "Renderer/Camera.h"
#include "Input/Input.h"
#include <GLFW/glfw3.h>
#include <cmath>



static unsigned int VAO = 0;
static Shader* s_Shader = nullptr;
static Shader* s_PickingShader = nullptr;  // writes integer entity ID
static Camera* s_Camera = nullptr;
static unsigned int s_FBO = 0;
static unsigned int s_ColorTexture = 0;
static int s_FBWidth = 1280;
static int s_FBHeight = 720;
static int s_SelectedEntity = -1;
static unsigned int s_PickingFBO = 0;
static unsigned int s_PickingTexture = 0;
static unsigned int gridVAO = 0;
static unsigned int gridVBO = 0;
static std::vector<float> grid;
static unsigned int gridShader = 0;

static unsigned int CompileGridShader(const char* vertexSrc, const char* fragmentSrc) {
    unsigned int program = glCreateProgram();
    unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
    unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert, 1, &vertexSrc, nullptr);
    glCompileShader(vert);

    int success = 0;
    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[1024];
        glGetShaderInfoLog(vert, 1024, nullptr, info);
        printf("Grid vertex shader compile failed: %s\n", info);
    }

    glShaderSource(frag, 1, &fragmentSrc, nullptr);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[1024];
        glGetShaderInfoLog(frag, 1024, nullptr, info);
        printf("Grid fragment shader compile failed: %s\n", info);
    }

    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info[1024];
        glGetProgramInfoLog(program, 1024, nullptr, info);
        printf("Grid shader link failed: %s\n", info);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    return program;
}


void Renderer::Init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float vertices[] = {

        // positions      // tex coords

        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, 1.0f, 1.0f,

        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f
    };

    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 
        2, 
        GL_FLOAT, 
        GL_FALSE, 
        4 * sizeof(float), 
        (void*)0
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );
    const char* vs = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aTexCoord;
        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;
        out vec2 v_TexCoord;
        void main() {
            v_TexCoord = aTexCoord;
            gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 0.0, 1.0);

        }
    )";

    const char* fs = R"(
        #version 330 core
        out vec4 color;
        in vec2 v_TexCoord;
        uniform sampler2D u_Texture;
        void main() {
            color = texture(u_Texture, v_TexCoord);
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

    // --------------------
    // Unity-like grid (light grey)
    // --------------------
    // Vertex format: vec2 position + vec4 color (6 floats per vertex).
    const char* gridVS = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec4 aColor;
        uniform mat4 u_ViewProjection;
        out vec4 vColor;
        void main() {
            gl_Position = u_ViewProjection * vec4(aPos, 0.0, 1.0);
            vColor = aColor;
        }
    )";

    const char* gridFS = R"(
        #version 330 core
        in vec4 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vColor;
        }
    )";

    gridShader = CompileGridShader(gridVS, gridFS);

    grid.clear();

    // Grid spans a fixed world-size; camera pan/zoom moves it via u_ViewProjection.
    const float halfSize = 20.0f;
    const float majorStep = 1.0f;    // darker lines every 1 unit
    const float minorStep = 0.25f;   // lighter lines every 0.25 unit
    const int majorDiv = (int)(majorStep / minorStep); // expected 4

    const glm::vec4 minorColor(0.80f, 0.80f, 0.80f, 0.45f);
    const glm::vec4 majorColor(0.62f, 0.62f, 0.62f, 0.65f);
    const glm::vec4 axisColor(0.52f, 0.52f, 0.52f, 0.85f);

    const int minorCount = (int)round(halfSize / minorStep);

    auto pushVertex = [&](float x, float y, const glm::vec4& c) {
        grid.push_back(x);
        grid.push_back(y);
        grid.push_back(c.r);
        grid.push_back(c.g);
        grid.push_back(c.b);
        grid.push_back(c.a);
    };

    // Vertical lines (X constant)
    for (int i = -minorCount; i <= minorCount; i++) {
        float x = i * minorStep;
        glm::vec4 c =
            (i == 0) ? axisColor : ((i % majorDiv) == 0 ? majorColor : minorColor);
        pushVertex(x, -halfSize, c);
        pushVertex(x,  halfSize, c);
    }

    // Horizontal lines (Y constant)
    for (int i = -minorCount; i <= minorCount; i++) {
        float y = i * minorStep;
        glm::vec4 c =
            (i == 0) ? axisColor : ((i % majorDiv) == 0 ? majorColor : minorColor);
        pushVertex(-halfSize, y, c);
        pushVertex( halfSize, y, c);
    }

    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, grid.size() * sizeof(float), grid.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

}
void Renderer::DrawGrid()
{
    if (gridShader == 0 || grid.empty())
        return;

    glUseProgram(gridShader);

    glm::mat4 vp = s_Camera->GetViewProjection();
    glUniformMatrix4fv(
        glGetUniformLocation(gridShader, "u_ViewProjection"),
        1,
        GL_FALSE,
        &vp[0][0]
    );

    glBindVertexArray(gridVAO);
    const int vertexCount = (int)(grid.size() / 6); // 6 floats per vertex
    glDrawArrays(GL_LINES, 0, vertexCount);
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
void Renderer::DrawTriangle(
    const glm::mat4& transform,
    unsigned int textureID)
{
    s_Shader->Bind();
    glActiveTexture(GL_TEXTURE0);
    s_Shader->SetInt("u_Texture", 0);

    s_Shader->SetMat4(
        "u_Transform",
        transform
    );

    glBindVertexArray(VAO);

    glBindTexture(
        GL_TEXTURE_2D,
        textureID
    );

    glDrawArrays(GL_TRIANGLES, 0, 6);
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
void Renderer::SetSelectedEntity(int entity)
{
    s_SelectedEntity = entity;
}

int Renderer::GetSelectedEntity()
{
    return s_SelectedEntity;
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
