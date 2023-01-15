#pragma once

#include <memory>

class RHIResource
{

};

class RHIBuffer : RHIResource
{};
class RHIBufferView : RHIResource
{};
class RHICommandBuffer : RHIResource
{};
class RHICommandPool : RHIResource
{};
class RHIDescriptorPool : RHIResource
{};
class RHIDescriptorSet : RHIResource
{};
class RHIDescriptorSetLayout : RHIResource
{};
class RHIDevice : RHIResource
{};
class RHIDeviceMemory : RHIResource
{};
class RHIEvent : RHIResource
{};
class RHIFence : RHIResource
{};
class RHIFramebuffer : RHIResource
{};
class RHIImage : RHIResource
{};
class RHIImageView : RHIResource
{};
class RHIInstance : RHIResource
{};
class RHIQueue : RHIResource
{};
class RHIPhysicalDevice : RHIResource
{};
class RHIPipeline : RHIResource
{};
class RHIPipelineCache : RHIResource
{};
class RHIPipelineLayout : RHIResource
{};
class RHIRenderPass : RHIResource
{};
class RHISampler : RHIResource
{};
class RHISemaphore : RHIResource
{};

class RHIShader : RHIResource
{};
class RHITexture : RHIResource
{};
class RHIUniformBuffer : RHIResource
{};
class RHIIndexBuffer : RHIResource
{};
class RHIVertexBuffer : RHIResource
{};

typedef std::shared_ptr<RHIUniformBuffer> RHIUniformBufferRef;
typedef std::shared_ptr<RHIIndexBuffer>   RHIIndexBufferRef;
typedef std::shared_ptr<RHIVertexBuffer>  RHIVertexBufferRef;


typedef std::shared_ptr<RHITexture> RHITextureRef;

typedef std::shared_ptr<RHIShader> RHIShaderRef;
