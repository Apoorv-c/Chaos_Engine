#include "Commands/SpawnCommand.h"
#include "Scene/Scene.h"

SpawnCommand::SpawnCommand(Scene* scene, const glm::vec3& position)
    : m_Scene(scene), m_Position(position), m_EntityID(0), m_HasEntityID(false)
{
}

void SpawnCommand::Execute()
{
    if (!m_HasEntityID)
    {
        m_EntityID = m_Scene->SpawnEntity(m_Position);
        m_HasEntityID = true;
    }
    else
    {
        // Re-spawn: reuse the same entity ID to maintain consistent references
        if (m_EntityID < m_Scene->GetEntities().size())
        {
            m_Scene->GetTransform(m_EntityID).Position = m_Position;
            m_Scene->GetRender(m_EntityID).Visible = true;
            m_Scene->RemoveFromFreeList(m_EntityID);
        }
    }
}

void SpawnCommand::Undo()
{
    if (m_HasEntityID)
    {
        m_Scene->DestroyEntity(m_EntityID);
    }
}
