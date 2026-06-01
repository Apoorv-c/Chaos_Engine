#pragma once

#include "Commands/Command.h"
#include <glm/glm.hpp>
#include "Scene/Entity.h"

class Scene;

class SpawnCommand : public Command
{
public:
    SpawnCommand(Scene* scene, const glm::vec3& position);

    void Execute() override;
    void Undo() override;

private:
    Scene* m_Scene;
    glm::vec3 m_Position;
    Entity m_EntityID;
    bool m_HasEntityID;
};
