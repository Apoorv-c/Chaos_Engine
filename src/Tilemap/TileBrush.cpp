#include "TileBrush.h"

bool        TileBrush::Active      = false;
std::string TileBrush::TexturePath = "";
std::string TileBrush::DisplayName = "";
int         TileBrush::TileID      = 1;

void TileBrush::Set(const std::string& path, const std::string& name, int id)
{
    TexturePath = path;
    DisplayName = name;
    TileID      = id;
    Active      = true;
}

void TileBrush::Clear()
{
    Active      = false;
    TexturePath = "";
    DisplayName = "";
    TileID      = 1;
}
