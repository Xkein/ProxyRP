#pragma once

#include "rhi_resource.h"

class RenderResource
{
public:
    virtual void InitRHI() {}

    virtual void ReleaseRHI() {}
};


class Texture : RenderResource
{
public:
    RHITextureRef RHITexture;
};

class VertexBuffer : RenderResource
{


};

typedef std::shared_ptr<Texture> TextureRef;

