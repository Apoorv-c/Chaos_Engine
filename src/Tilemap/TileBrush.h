#pragma once

#include <string>

struct TileBrush
{
    static bool        Active;
    static std::string TexturePath;
    static std::string DisplayName;
    static int         TileID;

    static void Set(const std::string& path, const std::string& name, int id = 1);
    static void Clear();
};
