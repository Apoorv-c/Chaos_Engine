#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Systems/RenderSystem.h"
#include "Systems/TransformSystem.h"
#include <vector>
#include <queue>
#include <string>

class Scene {
public:
    Scene();

    void OnUpdate(float deltaTime);
    void OnRender();
    void Save(const std::string& path);
    void Load(const std::string& path);
    void Clear();
    Entity CreateEntity();
    Entity SpawnEntity(const glm::vec3& position);
    TransformComponent& GetTransform(Entity e);
    RenderComponent& GetRender(Entity e);
    const std::vector<Entity>& GetEntities() const { return m_Entities; }
    std::vector<TransformComponent>& GetTransforms() { return m_Transforms; }
    const std::vector<RenderComponent>& GetRenderables() const { return m_Renderables; }
    void DestroyEntity(Entity e);
    void RemoveFromFreeList(Entity e);

private:
    std::vector<Entity> m_Entities;
    std::vector<TransformComponent> m_Transforms;
    std::vector<RenderComponent> m_Renderables;
    float m_Rotation = 0.0f;
    std::queue<Entity> m_FreeEntities;
};
