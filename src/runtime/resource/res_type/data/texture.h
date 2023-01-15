#pragma once

#include "core/reflection/macro.h"
#include "platform/platform.h"
#include "platform/string.h"

class TextureData
{
    friend class TextureLoader;
public:
    ~TextureData();

    int GetMipLevels();

    int GetSize() { return Width * Height * 4; }

    byte* GetData() { return Data; }

    int Width;
    int Height;
    int Channels;

private:
    byte* Data;
};

STRUCT()
struct TextureResource
{
    PROPERTY()
    String Path;
};
