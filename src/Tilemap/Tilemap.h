#pragma once

#include <vector>
#include "Tile.h"

class Tilemap
{
public:

    Tilemap(int width, int height);

    Tile& GetTile(int x, int y);

    void SetTile(int x, int y, const Tile& tile);

    int GetWidth() const;
    int GetHeight() const;

private:

    int m_Width;
    int m_Height;

    std::vector<Tile> m_Tiles;
};