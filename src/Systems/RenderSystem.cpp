#include "Systems/RenderSystem.h"
#include "Renderer/Renderer.h"

void RenderSystem::Render(
    const std::vector<Entity>& entities,
    const std::vector<TransformComponent>& transforms,
    const std::vector<RenderComponent>& renderables)
{
    for (size_t i = 0; i < entities.size(); i++) {
        if (!renderables[i].Visible)
            continue;

        bool selected = ((int)i == Renderer::GetSelectedEntity());

        Renderer::DrawTriangle(
            transforms[i].GetMatrix(),
            selected
        );
    }
}
