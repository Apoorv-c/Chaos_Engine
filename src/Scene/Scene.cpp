#include "Scene/Scene.h"
#include "Renderer/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene() {
    // Entity left;
    // left.TransformComponent.Position = {-0.7f, 0.0f, 0.0f};
    // m_Entities.push_back(left);

    // Entity center;
    // m_Entities.push_back(center);

    // Entity right;
    // right.TransformComponent.Position = {0.7f, 0.0f, 0.0f};
    // m_Entities.push_back(right);

    for (int i = 0; i < 3; i++) {
        Entity e = (Entity)m_Entities.size();
        m_Entities.push_back(e);
        m_Transforms.emplace_back();
    }

    m_Transforms[0].Position = {-0.7f, 0.0f, 0.0f};
    m_Transforms[2].Position = {0.7f, 0.0f, 0.0f};
}


void Scene::OnUpdate(float deltaTime) {
    m_Rotation += deltaTime;

    // Rotate middle object
    m_Transforms[1].Rotation = m_Rotation;
}
void Scene::OnRender() {
    for (size_t i = 0; i < m_Entities.size(); i++) {
        Renderer::DrawTriangle(m_Transforms[i].GetMatrix());
    }
}

