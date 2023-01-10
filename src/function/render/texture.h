#pragma once

#include "platform/platform.h"
#include "function/render/render_resources.h"
#include "function/render/rhi_resources.h"

#include <memory>


class Texture : RenderResource
{
public:
    RHITextureRef RHITexture;

};

typedef std::shared_ptr<Texture> TextureRef;
