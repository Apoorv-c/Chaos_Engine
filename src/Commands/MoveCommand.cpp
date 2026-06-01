#include "Commands/MoveCommand.h"
#include "Scene/Scene.h"

MoveCommand::MoveCommand(
    Scene* scene,
    int entity,
    const glm::vec3& oldPos,
    const glm::vec3& newPos)
{
    m_Scene = scene;
    m_Entity = entity;

    m_OldPos = oldPos;
    m_NewPos = newPos;
}

void MoveCommand::Execute()
{
    m_Scene->GetTransform(m_Entity).Position = m_NewPos;
}

void MoveCommand::Undo()
{
    m_Scene->GetTransform(m_Entity).Position = m_OldPos;
}