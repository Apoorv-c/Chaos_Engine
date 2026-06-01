#pragma once

#include "Commands/Command.h"
#include <glm/glm.hpp>

class Scene;

class MoveCommand : public Command
{
public:
    MoveCommand(
        Scene* scene,
        int entity,
        const glm::vec3& oldPos,
        const glm::vec3& newPos
    );

    void Execute() override;
    void Undo() override;

private:
    Scene* m_Scene;

    int m_Entity;

    glm::vec3 m_OldPos;
    glm::vec3 m_NewPos;
};