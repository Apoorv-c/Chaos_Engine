#include "Tilemap.h"

Tilemap::Tilemap(int width, int height)
    : m_Width(width),
      m_Height(height)
{
    m_Tiles.resize(width * height);
}

Tile& Tilemap::GetTile(int x, int y)
{
    return m_Tiles[y * m_Width + x];
}

void Tilemap::SetTile(int x, int y, const Tile& tile)
{
    m_Tiles[y * m_Width + x] = tile;
}

int Tilemap::GetWidth() const
{
    return m_Width;
}

int Tilemap::GetHeight() const
{
    return m_Height;
}