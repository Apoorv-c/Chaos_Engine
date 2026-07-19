#include "TileRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Scene/Scene.h"
#include "Renderer/Renderer.h"
#include "Tilemap/TextureCache.h"

void TileRenderer::Render(Scene& scene)
{
    Tilemap& tilemap = scene.GetTilemap();

    for (int y = 0; y < tilemap.GetHeight(); y++)
    {
        for (int x = 0; x < tilemap.GetWidth(); x++)
        {
            Tile& tile = tilemap.GetTile(x, y);

            if (tile.ID == -1)
                continue;

            glm::mat4 transform =
                glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3(
                        (float)x,
                        (float)y,
                        0.0f
                    )
                );

            // Scale down slightly so tiles have a thin visible gap
            transform = glm::scale(transform, glm::vec3(0.98f, 0.98f, 1.0f));

            // Tile.TexturePath → TextureCache → OpenGL ID → DrawQuad
            unsigned int texID = TextureCache::Get(tile.TexturePath);

            Renderer::DrawQuad(transform, texID);
        }
    }
}