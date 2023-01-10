#include "texture_loader.h"
#include "platform/file/file_manager.h"

#include <stb_image.h>
#include <stdexcept>
#include <map>

std::map<String, std::weak_ptr<TextureData>> g_loaded_textures;

std::shared_ptr<TextureData> TextureLoader::LoadData(const Char* path)
{
    // find from cache.
    if (auto it = g_loaded_textures.find(path); it != g_loaded_textures.end())
    {
        if (!it->second.expired())
        {
            return it->second.lock();
        }
    }

    // cache miss. create and store texture.

    std::shared_ptr<TextureData> texture = std::shared_ptr<TextureData>(new TextureData());

    std::vector<byte> data = FileManager::Read(path);

    byte* pixels = stbi_load_from_memory(data.data(), data.size(), &texture->Width, &texture->Height, &texture->Channels, STBI_rgb_alpha);

    if (pixels == nullptr)
    {
        throw std::runtime_error("failed to load texture!");
    }
    texture->Data = pixels;

    g_loaded_textures[path] = texture;

    return texture;
}

TextureResource TextureLoader::LoadResource(const Char* path)
{
    TextureResource texture;
    texture.Path = path;
    return texture;
}
