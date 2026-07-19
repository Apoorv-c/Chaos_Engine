#include "Systems/RenderSystem.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture2D.h"
#include <unordered_map>
#include <iostream>

void RenderSystem::Render(
    const std::vector<Entity>& entities,
    const std::vector<TransformComponent>& transforms,
    const std::vector<RenderComponent>& renderables,
    const std::vector<TextureComponent>& textures)
{
    for (size_t i = 0; i < entities.size(); i++) {
        if (!renderables[i].Visible)
            continue;

        bool selected = ((int)i == Renderer::GetSelectedEntity());

        unsigned int textureID = 0;

        const auto& texture = textures[i];

        if (!texture.Path.empty())
        {
            static std::unordered_map<
                std::string,
                Texture2D*> textureCache;

            if (textureCache.find(texture.Path)
                == textureCache.end())
            {
                textureCache[texture.Path] =
                    new Texture2D(texture.Path);
            }

            textureID =
                textureCache[texture.Path]->GetID();
        }
        Renderer::DrawQuad(
            transforms[i].GetMatrix(),
            textureID
        );
    }
}
