#include "Renderer/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float left, float right, float bottom, float top) {
    m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    Recalculate();
}

void Camera::SetPosition(const glm::vec3& position) {
    m_Position = position;
    Recalculate();
}
void Camera::SetProjection(float left, float right, float bottom, float top) {
    m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    Recalculate();
}

const glm::mat4& Camera::GetViewProjection() const {
    return m_ViewProjection;
}

void Camera::Recalculate() {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);
    m_View = glm::inverse(transform);
    m_ViewProjection = m_Projection * m_View;
}
