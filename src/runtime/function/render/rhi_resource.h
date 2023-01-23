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


typedef std::shared_ptr<RHIBuffer>              RHIBufferRef;
typedef std::shared_ptr<RHIBufferView>          RHIBufferViewRef;
typedef std::shared_ptr<RHICommandBuffer>       RHICommandBufferRef;
typedef std::shared_ptr<RHICommandPool>         RHICommandPoolRef;
typedef std::shared_ptr<RHIDescriptorPool>      RHIDescriptorPoolRef;
typedef std::shared_ptr<RHIDescriptorSet>       RHIDescriptorSetRef;
typedef std::shared_ptr<RHIDescriptorSetLayout> RHIDescriptorSetLayoutRef;
typedef std::shared_ptr<RHIDevice>              RHIDeviceRef;
typedef std::shared_ptr<RHIDeviceMemory>        RHIDeviceMemoryRef;
typedef std::shared_ptr<RHIEvent>               RHIEventRef;
typedef std::shared_ptr<RHIFence>               RHIFenceRef;
typedef std::shared_ptr<RHIFramebuffer>         RHIFramebufferRef;
typedef std::shared_ptr<RHIImage>               RHIImageRef;
typedef std::shared_ptr<RHIImageView>           RHIImageViewRef;
typedef std::shared_ptr<RHIInstance>            RHIInstanceRef;
typedef std::shared_ptr<RHIQueue>               RHIQueueRef;
typedef std::shared_ptr<RHIPhysicalDevice>      RHIPhysicalDeviceRef;
typedef std::shared_ptr<RHIPipeline>            RHIPipelineRef;
typedef std::shared_ptr<RHIPipelineCache>       RHIPipelineCacheRef;
typedef std::shared_ptr<RHIPipelineLayout>      RHIPipelineLayoutRef;
typedef std::shared_ptr<RHIRenderPass>          RHIRenderPassRef;
typedef std::shared_ptr<RHISampler>             RHISamplerRef;
typedef std::shared_ptr<RHISemaphore>           RHISemaphoreRef;
typedef std::shared_ptr<RHIShader>              RHIShaderRef;

