#include "Systems/SystemManager.h"
#include "Systems/TransformSystem.h"
#include "Systems/RenderSystem.h"

void SystemManager::Update(Scene& scene, float deltaTime) {
    TransformSystem::Update(
        deltaTime,
        scene.GetEntities(),
        scene.GetTransforms()
    );
}

void SystemManager::Render(Scene& scene) {
    RenderSystem::Render(
        scene.GetEntities(),
        scene.GetTransforms(),
        scene.GetRenderables()
    );
}
