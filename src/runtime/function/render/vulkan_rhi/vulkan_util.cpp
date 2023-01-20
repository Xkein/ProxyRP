#include "vulkan_util.h"
#include "core/log/log_system.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"

uint32_t
VulkanUtil::FindMemoryType(vk::PhysicalDevice physical_device, uint32_t type_filter, vk::MemoryPropertyFlags properties)
{
    vk::PhysicalDeviceMemoryProperties memory_properties = physical_device.getMemoryProperties();

    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
    {
        if ((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    
    LOG_ERROR("failed to find suitable memory type!");
    return 0;
}

vk::SampleCountFlagBits VulkanUtil::GetMaxUsableSampleCount(vk::PhysicalDevice physical_device)
{
    vk::PhysicalDeviceProperties physical_device_properties = physical_device.getProperties();

    vk::SampleCountFlags counts = physical_device_properties.limits.framebufferColorSampleCounts &
                                  physical_device_properties.limits.framebufferDepthSampleCounts;

    if (counts & vk::SampleCountFlagBits::e64)
        return vk::SampleCountFlagBits::e64;
    if (counts & vk::SampleCountFlagBits::e32)
        return vk::SampleCountFlagBits::e32;
    if (counts & vk::SampleCountFlagBits::e16)
        return vk::SampleCountFlagBits::e16;
    if (counts & vk::SampleCountFlagBits::e8)
        return vk::SampleCountFlagBits::e8;
    if (counts & vk::SampleCountFlagBits::e4)
        return vk::SampleCountFlagBits::e4;
    if (counts & vk::SampleCountFlagBits::e2)
        return vk::SampleCountFlagBits::e2;

    return vk::SampleCountFlagBits::e1;
}

vk::Format VulkanUtil::FindSupportedFormat(vk::PhysicalDevice             physical_device,
                                           const std::vector<vk::Format>& candidates,
                                           vk::ImageTiling                tiling,
                                           vk::FormatFeatureFlags         features)
{

    for (vk::Format format : candidates)
    {
        vk::FormatProperties properties = physical_device.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
        {
            return format;
        }

        if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    LOG_ERROR("failed to find supported format");
    return vk::Format();
}

vk::Format VulkanUtil::FindDepthFormat(vk::PhysicalDevice physical_device)
{
    return FindSupportedFormat(physical_device,
                               {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
                               vk::ImageTiling::eOptimal,
                               vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

bool VulkanUtil::HasStencilComponent(vk::Format format)
{
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

vk::ShaderModule VulkanUtil::CreateShaderModule(vk::Device device, const std::vector<byte>& shader_code)
{
    vk::ShaderModuleCreateInfo create_info {
        .codeSize = shader_code.size(),
        .pCode    = (const uint32_t*)shader_code.data(),
    };

    vk::ShaderModule shader_module = device.createShaderModule(create_info);
    return shader_module;
}

void VulkanUtil::CreateImage(vk::PhysicalDevice      physical_device,
                             vk::Device              device,
                             uint32_t                width,
                             uint32_t                height,
                             vk::Format              format,
                             vk::ImageTiling         tiling,
                             vk::ImageUsageFlags     usage,
                             vk::MemoryPropertyFlags properties,
                             vk::Image&              image,
                             vk::DeviceMemory&       image_memory,
                             vk::ImageCreateFlags    create_flags,
                             uint32_t                array_layers,
                             uint32_t                mip_levels,
                             vk::SampleCountFlagBits sample_count)
{
    vk::ImageCreateInfo image_info {
        .imageType     = vk::ImageType::e2D,
        .format        = format,
        .extent        = {width, height, 1},
        .mipLevels     = mip_levels,
        .arrayLayers   = array_layers,
        .samples       = sample_count,
        .tiling        = tiling,
        .usage         = usage,
        .sharingMode   = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    image = device.createImage(image_info);

    vk::MemoryRequirements memory_requirements = device.getImageMemoryRequirements(image);

    vk::MemoryAllocateInfo alloc_info {
        .allocationSize  = memory_requirements.size,
        .memoryTypeIndex = FindMemoryType(physical_device, memory_requirements.memoryTypeBits, properties),
    };

    image_memory = device.allocateMemory(alloc_info);

    device.bindImageMemory(image, image_memory, 0);
}

vk::ImageView VulkanUtil::CreateImageView(vk::Device           device,
                                          vk::Image            image,
                                          vk::Format           format,
                                          vk::ImageAspectFlags aspect_flags,
                                          vk::ImageViewType    view_type,
                                          uint32_t             layout_count,
                                          uint32_t             mip_levels)
{
    vk::ImageSubresourceRange subresource_range {
        .aspectMask     = aspect_flags,
        .baseMipLevel   = 0,
        .levelCount     = mip_levels,
        .baseArrayLayer = 0,
        .layerCount     = layout_count,
    };

    vk::ImageViewCreateInfo view_info {
        .image            = image,
        .viewType         = view_type,
        .format           = format,
        .subresourceRange = subresource_range,
    };

    return device.createImageView(view_info);
}

void VulkanUtil::TransitionImageLayout(VulkanRHI*           rhi,
                                       vk::Image            image,
                                       vk::Format           format,
                                       vk::ImageLayout      old_layout,
                                       vk::ImageLayout      new_layout,
                                       vk::ImageAspectFlags aspect_mask_bits,
                                       uint32_t             layer_count,
                                       uint32_t             mip_levels)
{
    vk::CommandBuffer command_buffer = rhi->BeginSingleTimeCommands();

    vk::ImageSubresourceRange sub_resource_range {
        .aspectMask     = aspect_mask_bits,
        .baseMipLevel   = 0,
        .levelCount     = mip_levels,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    vk::ImageMemoryBarrier barrier {
        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = image,
        .subresourceRange    = sub_resource_range,
    };

    vk::PipelineStageFlags source_stage;
    vk::PipelineStageFlags destination_stage;

    if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        source_stage      = vk::PipelineStageFlagBits::eTopOfPipe;
        destination_stage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (old_layout == vk::ImageLayout::eTransferDstOptimal &&
             new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        source_stage      = vk::PipelineStageFlagBits::eTransfer;
        destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask =
            vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        source_stage      = vk::PipelineStageFlagBits::eTopOfPipe;
        destination_stage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else
    {
        LOG_ERROR("unsupported layout transition!");
        return;
    }

    command_buffer.pipelineBarrier(source_stage, destination_stage, {}, {}, {}, {barrier});

    rhi->EndSingleTimeCommands(command_buffer);
}
