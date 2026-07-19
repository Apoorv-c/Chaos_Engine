#pragma once

#include <string>
#include <unordered_map>
#include "Renderer/Texture2D.h"

class TextureCache
{
public:
    // Returns the OpenGL texture ID for the given path.
    // Loads and caches the texture on first request.
    static unsigned int Get(const std::string& path);

    // Free all cached textures (call on shutdown).
    static void Clear();

private:
    static std::unordered_map<std::string, Texture2D*> s_Cache;
};
