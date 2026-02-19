#pragma once
#include <vector>
#include "Scene/Entity.h"
#include "Scene/Components.h"

class TransformSystem {
public:
    static void Update(
        float deltaTime,
        const std::vector<Entity>& entities,
        std::vector<TransformComponent>& transforms
    );
};
