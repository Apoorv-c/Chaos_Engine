#include "Commands/DestroyCommand.h"
#include "Scene/Scene.h"

DestroyCommand::DestroyCommand(Scene* scene, Entity entity)
    : m_Scene(scene), m_EntityID(entity)
{
    m_SavedTransform = m_Scene->GetTransform(m_EntityID);
    m_SavedRender = m_Scene->GetRender(m_EntityID);
    m_SavedTexture = m_Scene->GetTexture(m_EntityID);
}

void DestroyCommand::Execute()
{
    m_Scene->DestroyEntity(m_EntityID);
}

void DestroyCommand::Undo()
{
    if (m_EntityID < m_Scene->GetEntities().size())
    {
        m_Scene->GetTransform(m_EntityID) = m_SavedTransform;
        m_Scene->GetRender(m_EntityID) = m_SavedRender;
        m_Scene->GetRender(m_EntityID).Visible = true;
        m_Scene->GetTexture(m_EntityID) = m_SavedTexture;
        m_Scene->RemoveFromFreeList(m_EntityID);
    }
}
