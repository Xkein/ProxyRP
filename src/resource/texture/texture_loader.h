#pragma once

#include "resource/texture/texture_resource.h"

#include <memory>

class TextureLoader
{
public:
    static std::shared_ptr<TextureData> LoadData(const Char* path);

    static TextureResource LoadResource(const Char* path);
};

