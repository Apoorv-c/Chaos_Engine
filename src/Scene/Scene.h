#pragma once
#include <vector>
#include <glm/glm.hpp>

class Scene {
public:
    Scene();

    void OnUpdate(float deltaTime);
    void OnRender();

private:
    std::vector<glm::mat4> m_Objects;
    float m_Rotation = 0.0f;
};
