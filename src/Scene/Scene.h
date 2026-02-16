#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Systems/RenderSystem.h"

#include <vector>

class Scene {
public:
    Scene();

    void OnUpdate(float deltaTime);
    void OnRender();

    Entity CreateEntity();

    TransformComponent& GetTransform(Entity e);
    RenderComponent& GetRender(Entity e);

private:
    std::vector<Entity> m_Entities;
    std::vector<TransformComponent> m_Transforms;
    std::vector<RenderComponent> m_Renderables;
    float m_Rotation = 0.0f;
};
