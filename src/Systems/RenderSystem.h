#pragma once
#include <vector>
#include "Scene/Entity.h"
#include "Scene/Components.h"

class RenderSystem {
public:
    static void Render(
        const std::vector<Entity>& entities,
        const std::vector<TransformComponent>& transforms,
        const std::vector<RenderComponent>& renderables
    );
};
