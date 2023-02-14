#include "vulkan_rhi_resources.h"
#include "core/log/log_system.h"
#include "function/render/rhi_definition.h"
#include "function/render/rhi_struct.h"

template<typename RHIType>
class VulkanRHIConverterTrait
{
public:
    using VKType = void;
};

class VulkanRHIConverter
{
public:
    template<typename RHIType, typename VKNativeType = VulkanRHIConverterTrait<RHIType>::VKNativeType>
    static VKNativeType Convert(const RHIType& rhi_data)
    {
        // we use Vulkan's type in fact.
        return *(VKNativeType*)&rhi_data;
    }

    template<typename RHIType, typename VKNativeType = VulkanRHIConverterTrait<RHIType>::VKNativeType>
    static std::vector<VKNativeType> Convert(const std::span<RHIType*>& rhi_span)
    {
        std::vector<VKNativeType> vk_vector(rhi_span.size());
        for (size_t i = 0; i < rhi_span.size(); i++)
        {
            vk_vector[i] = Convert(*rhi_span[i]);
        }
        return vk_vector;
    }

    template<size_t count, typename RHIType, typename VKNativeType = VulkanRHIConverterTrait<RHIType>::VKNativeType>
    static std::array<VKNativeType, count> Convert(const std::array<RHIType*, count>& rhi_span)
    {
        std::array<VKNativeType, count> vk_array;
        for (size_t i = 0; i < rhi_span.size(); i++)
        {
            vk_array[i] = Convert(*rhi_span[i]);
        }
        return vk_array;
    }

    template<size_t count, typename RHIType, typename VKNativeType = VulkanRHIConverterTrait<RHIType>::VKNativeType>
    static std::vector<VKNativeType> Convert(const std::vector<RHIType*>& rhi_span)
    {
        return Convert(std::span(rhi_span));
    }
};

#define __DEFINE_VULKAN_RHI_CONVERTER(Type) \
template<> \
inline vk::Type VulkanRHIConverter::Convert(const RHI##Type& rhi_data) \
{ \
    return *(Vulkan##Type*)&rhi_data; \
}

#define __DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Type) \
template<> \
class VulkanRHIConverterTrait<RHI##Type> \
{ \
public: \
    using VKNativeType = vk::Type; \
}

__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(RenderPassCreateInfo);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(FramebufferCreateInfo);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(DescriptorSetLayoutCreateInfo);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(PipelineLayoutCreateInfo);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(GraphicsPipelineCreateInfo);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(DescriptorSetAllocateInfo);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(DescriptorPoolCreateInfo);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(RenderPassBeginInfo);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(CommandBufferAllocateInfo);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(CommandBufferBeginInfo);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(SamplerCreateInfo);

__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Buffer);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(BufferView);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(CommandBuffer);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(CommandPool);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(DescriptorPool);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(DescriptorSet);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(DescriptorSetLayout);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Device);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(DeviceMemory);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Event);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Fence);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Framebuffer);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Image);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(ImageView);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Instance);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Queue);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(PhysicalDevice);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Pipeline);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(PipelineCache);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(PipelineLayout);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(RenderPass);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Sampler);
__DEFINE_VULKAN_RHI_NATIVE_CONVERTER_TRAIT(Semaphore);

__DEFINE_VULKAN_RHI_CONVERTER(Buffer);
__DEFINE_VULKAN_RHI_CONVERTER(BufferView);
__DEFINE_VULKAN_RHI_CONVERTER(CommandBuffer);
__DEFINE_VULKAN_RHI_CONVERTER(CommandPool);
__DEFINE_VULKAN_RHI_CONVERTER(DescriptorPool);
__DEFINE_VULKAN_RHI_CONVERTER(DescriptorSet);
__DEFINE_VULKAN_RHI_CONVERTER(DescriptorSetLayout);
__DEFINE_VULKAN_RHI_CONVERTER(Device);
__DEFINE_VULKAN_RHI_CONVERTER(DeviceMemory);
__DEFINE_VULKAN_RHI_CONVERTER(Event);
__DEFINE_VULKAN_RHI_CONVERTER(Fence);
__DEFINE_VULKAN_RHI_CONVERTER(Framebuffer);
__DEFINE_VULKAN_RHI_CONVERTER(Image);
__DEFINE_VULKAN_RHI_CONVERTER(ImageView);
__DEFINE_VULKAN_RHI_CONVERTER(Instance);
__DEFINE_VULKAN_RHI_CONVERTER(Queue);
__DEFINE_VULKAN_RHI_CONVERTER(PhysicalDevice);
__DEFINE_VULKAN_RHI_CONVERTER(Pipeline);
__DEFINE_VULKAN_RHI_CONVERTER(PipelineCache);
__DEFINE_VULKAN_RHI_CONVERTER(PipelineLayout);
__DEFINE_VULKAN_RHI_CONVERTER(RenderPass);
__DEFINE_VULKAN_RHI_CONVERTER(Sampler);
__DEFINE_VULKAN_RHI_CONVERTER(Semaphore);

template<>
class VulkanRHIConverterTrait<RHIShader>
{
public:
    using VKNativeType = vk::ShaderModule;
};
template<>
inline vk::ShaderModule VulkanRHIConverter::Convert(const RHIShader& rhi_data)
{
    return *(VulkanShader*)&rhi_data;
}

