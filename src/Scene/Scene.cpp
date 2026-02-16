#include "Scene/Scene.h"
#include "Renderer/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene() {

    Entity left = CreateEntity();
    Entity middle = CreateEntity();
    Entity right = CreateEntity();
    

    GetTransform(left).Position = {-0.7f, 0.0f, 0.0f};
    GetTransform(right).Position = {0.7f, 0.0f, 0.0f};
}


void Scene::OnUpdate(float deltaTime) {
    m_Rotation += deltaTime;

    // Rotate middle object
    GetTransform(1).Rotation = m_Rotation;
}
void Scene::OnRender() {
    for (size_t i = 0; i < m_Entities.size(); i++) {
        if(!m_Renderables[i].Visible)
            continue;

        Renderer::DrawTriangle(m_Transforms[i].GetMatrix());
    }
}


Entity Scene::CreateEntity() {
    Entity e = (Entity)m_Entities.size();
    m_Entities.push_back(e);

    m_Transforms.emplace_back();
    m_Renderables.emplace_back();

    return e;
}

TransformComponent& Scene::GetTransform(Entity e) {
    return m_Transforms[e];
}

RenderComponent& Scene::GetRender(Entity e) {
    return m_Renderables[e];
}
