#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Time.h"
#include "Renderer/Camera.h"
#include "Input/Input.h"
#include "Renderer/Camera.h"



static unsigned int VAO = 0;
static Shader* s_Shader = nullptr;
static Camera* s_Camera = nullptr;

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
    s_Camera = new Camera(-1.6f, 1.6f, -0.9f, 0.9f);

}


void Renderer::Shutdown() {
    // Placeholder for later
}

void Renderer::BeginFrame() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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

void Renderer::EndFrame() {
    // Nothing yet (swap handled by Window)
}
