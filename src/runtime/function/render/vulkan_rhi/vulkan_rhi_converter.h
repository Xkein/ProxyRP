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
    template<typename RHIType, typename VKType = VulkanRHIConverterTrait<RHIType>::VKType>
    static VKType Convert(const RHIType& rhi_data)
    {
        // we use Vulkan's type in face.
        return *(VKType*)&rhi_data;
    }
};


#define __DEFINE_VULKAN_RHI_CONVERTER_TRAIT(Type) \
template<> \
class VulkanRHIConverterTrait<RHI##Type> \
{ \
public: \
    using VKType = vk::Type; \
}

__DEFINE_VULKAN_RHI_CONVERTER_TRAIT(RenderPassCreateInfo);
__DEFINE_VULKAN_RHI_CONVERTER_TRAIT(FramebufferCreateInfo);
__DEFINE_VULKAN_RHI_CONVERTER_TRAIT(DescriptorSetLayoutCreateInfo);
__DEFINE_VULKAN_RHI_CONVERTER_TRAIT(PipelineLayoutCreateInfo);


