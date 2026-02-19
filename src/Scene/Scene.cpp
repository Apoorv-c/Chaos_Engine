#include "Scene/Scene.h"
#include "Renderer/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Systems/RenderSystem.h"


Scene::Scene() {

    Entity left = CreateEntity();
    Entity middle = CreateEntity();
    Entity right = CreateEntity();
    

    GetTransform(left).Position = {-0.7f, 0.0f, 0.0f};
    GetTransform(right).Position = {0.7f, 0.0f, 0.0f};
}


void Scene::OnUpdate(float deltaTime) {
    TransformSystem::Update(deltaTime, m_Entities, m_Transforms);
}

void Scene::OnRender() {
    RenderSystem::Render(m_Entities, m_Transforms, m_Renderables);
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
