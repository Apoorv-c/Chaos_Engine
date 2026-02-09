#include "Scene/Scene.h"
#include "Renderer/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene() {
    Entity left;
    left.TransformComponent.Position = {-0.7f, 0.0f, 0.0f};
    m_Entities.push_back(left);

    Entity center;
    m_Entities.push_back(center);

    Entity right;
    right.TransformComponent.Position = {0.7f, 0.0f, 0.0f};
    m_Entities.push_back(right);
}


void Scene::OnUpdate(float deltaTime) {
    m_Rotation += deltaTime;

    // Rotate middle object
    m_Entities[1].TransformComponent.Rotation = m_Rotation;
}

void Scene::OnRender() {
    for (auto& entity : m_Entities) {
        Renderer::DrawTriangle(entity.TransformComponent.GetMatrix());
    }
}
