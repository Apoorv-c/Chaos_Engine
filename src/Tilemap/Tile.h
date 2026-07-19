#pragma once

#include <string>

struct Tile
{
    int ID = -1;

    std::string TexturePath;

    unsigned int TextureID = 0;

    bool Walkable = true;
};