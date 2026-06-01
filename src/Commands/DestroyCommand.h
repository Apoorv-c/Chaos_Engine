#pragma once

#include "Commands/Command.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"

class Scene;

class DestroyCommand : public Command
{
public:
    DestroyCommand(Scene* scene, Entity entity);

    void Execute() override;
    void Undo() override;

private:
    Scene* m_Scene;
    Entity m_EntityID;
    TransformComponent m_SavedTransform;
    RenderComponent m_SavedRender;
};
