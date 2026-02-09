#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Scene/Entity.h"
#include <vector>

class Scene {
public:
    Scene();

    void OnUpdate(float deltaTime);
    void OnRender();

private:
    std::vector<Entity> m_Entities;
    float m_Rotation = 0.0f;
};
