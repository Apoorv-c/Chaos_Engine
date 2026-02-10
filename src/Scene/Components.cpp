#include "Scene/Components.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 TransformComponent::GetMatrix() const {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position);
    transform = glm::rotate(transform, Rotation, glm::vec3(0,0,1));
    transform = glm::scale(transform, Scale);
    return transform;
}
