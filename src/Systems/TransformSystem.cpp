#include "Systems/TransformSystem.h"

void TransformSystem::Update(
    float deltaTime,
    const std::vector<Entity>& entities,
    std::vector<TransformComponent>& transforms)
{
    static float rotation = 0.0f;
    rotation += deltaTime;

    // Rotate the second entity (for now)
    if (entities.size() > 1)
        transforms[1].Rotation = rotation;
}
