#pragma once

#include "resource/res_type/data/texture.h"

#include <memory>

class TextureLoader
{
public:
    static std::shared_ptr<TextureData> LoadData(const Char* path);

    static TextureResource LoadResource(const Char* path);
};

