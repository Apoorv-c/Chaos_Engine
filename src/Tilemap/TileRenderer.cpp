#include "TileRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Scene/Scene.h"
#include "Renderer/Renderer.h"

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

            // Choose color based on tile ID
            glm::vec4 color;
            switch (tile.ID)
            {
                case 1:  color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); break; // white
                default: color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); break; // grey fallback
            }

            Renderer::DrawColoredQuad(transform, color);
        }
    }
}