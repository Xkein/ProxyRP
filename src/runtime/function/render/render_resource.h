#pragma once

#include "rhi_resource.h"

class RenderResource
{
public:
    virtual ~RenderResource();

    virtual void InitRHI() {}

    virtual void ReleaseRHI() {}

    virtual void InitResource();

    virtual void ReleaseResource();

    virtual void UpdateRHI();
};


class Texture : RenderResource
{
public:
    RHIImageRef        ImageRHI;
    RHIImageViewRef    ImageViewRHI;
    RHIDeviceMemoryRef DeviceMemoryRHI;

    virtual void ReleaseRHI() override
    {
        ImageRHI.reset();
        ImageViewRHI.reset();
        DeviceMemoryRHI.reset();
    }
};

class VertexBuffer : RenderResource
{
public:
    RHIBufferRef       VertexBufferRHI;
    RHIDeviceMemoryRef DeviceMemoryRHI;

    virtual void ReleaseRHI() override {
        VertexBufferRHI.reset();
        DeviceMemoryRHI.reset();
    }
};

class IndexBuffer : RenderResource
{
public:
    RHIBufferRef       IndexBufferRHI;
    RHIDeviceMemoryRef DeviceMemoryRHI;

    virtual void ReleaseRHI() override
    {
        IndexBufferRHI.reset();
        DeviceMemoryRHI.reset();
    }
};

class Shader : RenderResource
{
public:
    RHIShaderRef ShaderRHI;

    virtual void ReleaseRHI() override
    {
        ShaderRHI.reset();
    }
};

typedef std::shared_ptr<Texture> TextureRef;

