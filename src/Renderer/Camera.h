#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    Camera(float left, float right, float bottom, float top);

    void SetPosition(const glm::vec3& position);
    void SetProjection(float left, float right, float bottom, float top);
    const glm::mat4& GetViewProjection() const;

private:
    void Recalculate();

private:
    glm::mat4 m_Projection;
    glm::mat4 m_View;
    glm::mat4 m_ViewProjection;

    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
};
