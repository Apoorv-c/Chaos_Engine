#include "Scene/Scene.h"
#include "Renderer/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene() {
    // Create 3 objects
    m_Objects.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(-0.7f, 0.0f, 0.0f)));
    m_Objects.push_back(glm::mat4(1.0f));
    m_Objects.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.0f, 0.0f)));
}

void Scene::OnUpdate(float deltaTime) {
    m_Rotation += deltaTime;

    // Rotate middle object
    m_Objects[1] = glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0,0,1));
}

void Scene::OnRender() {
    for (auto& transform : m_Objects) {
        Renderer::DrawTriangle(transform);
    }
}
