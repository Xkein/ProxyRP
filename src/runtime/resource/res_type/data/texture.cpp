#include "texture.h"

#include "stb_image.h"

#include <cmath>

TextureData::~TextureData()
{
    stbi_image_free(Data);
}

int TextureData::GetMipLevels()
{
    return std::floor(std::log2(std::max(Width, Height))) + 1;
}
