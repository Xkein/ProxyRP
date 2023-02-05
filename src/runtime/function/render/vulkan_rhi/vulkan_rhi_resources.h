#pragma once

#include "function/render/rhi_resource.h"

#include <optional>
#include <vulkan/vulkan.hpp>

#define __VULKAN_RHI_MAKE_OPERATOR() \
    using __resource_type = decltype(Resource); \
    using __resource_native_type = decltype(Resource)::NativeType; \
\
    inline auto& operator=(const __resource_type& other) { Resource = other; return *this; }\
\
    inline operator __resource_native_type () const { return Resource; } \
\
    inline operator __resource_type& () { return Resource; } \
    inline operator const __resource_type& () const { return Resource; } \
    inline __resource_type* operator->() { return &Resource; } \
    inline const __resource_type* operator->() const { return &Resource; }

class VulkanBuffer : public RHIBuffer
{
public:
    vk::Buffer Resource;

    VulkanBuffer(vk::Buffer buffer) : Resource(buffer) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanBufferView : public RHIBufferView
{
public:
    vk::BufferView Resource;

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanCommandBuffer : public RHICommandBuffer
{
public:
    vk::CommandBuffer Resource;

    VulkanCommandBuffer(vk::CommandBuffer command_buffer) : Resource(command_buffer) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanCommandPool : public RHICommandPool
{
public:
    vk::CommandPool Resource;

    VulkanCommandPool(vk::CommandPool command_pool) : Resource(command_pool) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanDescriptorPool : public RHIDescriptorPool
{
public:
    vk::DescriptorPool Resource;

    VulkanDescriptorPool(vk::DescriptorPool descriptor_pool) : Resource(descriptor_pool) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanDescriptorSet : public RHIDescriptorSet
{
public:
    vk::DescriptorSet Resource;

    VulkanDescriptorSet(vk::DescriptorSet descriptor_set) : Resource(descriptor_set) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanDescriptorSetLayout : public RHIDescriptorSetLayout
{
public:
    vk::DescriptorSetLayout Resource;

    VulkanDescriptorSetLayout(vk::DescriptorSetLayout descriptor_set_layout) : Resource(descriptor_set_layout) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanDevice : public RHIDevice
{
public:
    vk::Device Resource;

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanDeviceMemory : public RHIDeviceMemory
{
public:
    vk::DeviceMemory Resource;

    VulkanDeviceMemory(vk::DeviceMemory memory) : Resource(memory) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanEvent : public RHIEvent
{
public:
    vk::Event Resource;

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanFence : public RHIFence
{
public:
    vk::Fence Resource;

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanFramebuffer : public RHIFramebuffer
{
public:
    vk::Framebuffer Resource;

    VulkanFramebuffer(vk::Framebuffer framebuffer) : Resource(framebuffer) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanImage : public RHIImage
{
public:
    vk::Image Resource;

    VulkanImage(vk::Image image) : Resource(image) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanImageView : public RHIImageView
{
public:
    vk::ImageView Resource;

    VulkanImageView(vk::ImageView image_view) : Resource(image_view) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanInstance : public RHIInstance
{
public:
    vk::Instance Resource;
    
    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanQueue : public RHIQueue
{
public:
    vk::Queue Resource;

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanPhysicalDevice : public RHIPhysicalDevice
{
public:
    vk::PhysicalDevice Resource;

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanPipeline : public RHIPipeline
{
public:
    vk::Pipeline Resource;

    VulkanPipeline(vk::Pipeline pipeline) : Resource(pipeline) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanPipelineCache : public RHIPipelineCache
{
public:
    vk::PipelineCache Resource;

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanPipelineLayout : public RHIPipelineLayout
{
public:
    vk::PipelineLayout Resource;

    VulkanPipelineLayout(vk::PipelineLayout pipeline_layout) : Resource(pipeline_layout) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanRenderPass : public RHIRenderPass
{
public:
    vk::RenderPass Resource;

    VulkanRenderPass(vk::RenderPass render_pass) : Resource(render_pass) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanSampler : public RHISampler
{
public:
    vk::Sampler Resource;

    VulkanSampler(vk::Sampler sampler) : Resource(sampler) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanSemaphore : public RHISemaphore
{
public:
    vk::Semaphore Resource;

    __VULKAN_RHI_MAKE_OPERATOR();
};

class VulkanShader : public RHIShader
{
public:
    vk::ShaderModule Resource;

    VulkanShader(vk::ShaderModule shader_module) : Resource(shader_module) {}

    __VULKAN_RHI_MAKE_OPERATOR();
};


struct QueueFamilyIndices
{
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;
    std::optional<uint32_t> ComputeFamily;

    bool isComplete()
    {
        return GraphicsFamily.has_value() && PresentFamily.has_value() && ComputeFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    vk::SurfaceCapabilitiesKHR        Capabilities;
    std::vector<vk::SurfaceFormatKHR> Formats;
    std::vector<vk::PresentModeKHR>   PresentModes;
};
