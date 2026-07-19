#include "TextureCache.h"
#include "Core/Log.h"

std::unordered_map<std::string, Texture2D*> TextureCache::s_Cache;

unsigned int TextureCache::Get(const std::string& path)
{
    if (path.empty())
        return 0;

    auto it = s_Cache.find(path);
    if (it != s_Cache.end())
        return it->second->GetID();

    // Load new texture and cache it
    Texture2D* tex = new Texture2D(path);
    s_Cache[path] = tex;

    Log::Info("TextureCache: loaded " + path);

    return tex->GetID();
}

void TextureCache::Clear()
{
    for (auto& [path, tex] : s_Cache)
        delete tex;

    s_Cache.clear();
}
