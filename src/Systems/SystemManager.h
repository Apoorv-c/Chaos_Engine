#pragma once
#include "Scene/Scene.h"

class SystemManager {
public:
    static void Update(Scene& scene, float deltaTime);
    static void Render(Scene& scene);
};
