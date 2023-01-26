#pragma once

#include "rhi_resource.h"
#include "rhi_definition.h"
#include "platform/platform.h"

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
    RHIFormat          Format;

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

class UniformBuffer : RenderResource
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

class Pipeline : RenderResource
{
public:
    RHIPipelineRef       PipelineRHI;
    RHIPipelineLayoutRef LayoutRHI;

    virtual void ReleaseRHI() override
    {
        PipelineRHI.reset();
        LayoutRHI.reset();
    }
};

class Descriptor : RenderResource
{
public:
    RHIDescriptorSetRef       DescriptorSetRHI;
    RHIDescriptorSetLayoutRef LayoutRHI;

    virtual void ReleaseRHI() override
    {
        DescriptorSetRHI.reset();
        LayoutRHI.reset();
    }
};

typedef std::shared_ptr<Texture> TextureRef;

typedef std::shared_ptr<VertexBuffer> VertexBufferRef;
typedef std::shared_ptr<IndexBuffer>  IndexBufferRef;


