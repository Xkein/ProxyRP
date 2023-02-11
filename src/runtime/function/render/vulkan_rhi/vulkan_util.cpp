#include "vulkan_util.h"
#include "core/log/log_system.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "function/render/render_type.h"

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

vk::DeviceSize VulkanUtil::GetTextureSize(uint32_t width, uint32_t height, vk::Format format)
{
    vk::DeviceSize texture_size;
    switch (format)
    {
        case vk::Format::eR8G8B8Unorm:
            texture_size = width * height * 3;
            break;
        case vk::Format::eR8G8B8Srgb:
            texture_size = width * height * 3;
            break;
        case vk::Format::eR8G8B8A8Unorm:
            texture_size = width * height * 4;
            break;
        case vk::Format::eR8G8B8A8Srgb:
            texture_size = width * height * 4;
            break;
        case vk::Format::eR32Sfloat:
            texture_size = width * height * 4;
            break;
        case vk::Format::eR32G32Sfloat:
            texture_size = width * height * 4 * 2;
            break;
        case vk::Format::eR32G32B32Sfloat:
            texture_size = width * height * 4 * 3;
            break;
        case vk::Format::eR32G32B32A32Sfloat:
            texture_size = width * height * 4 * 4;
            break;
        default:
            LOG_ERROR("image format unsupported");
            break;
    }
    return texture_size;
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

void VulkanUtil::CreateBuffer(vk::PhysicalDevice      physical_device,
                              vk::Device              device,
                              vk::DeviceSize          size,
                              vk::BufferUsageFlags    usage,
                              vk::MemoryPropertyFlags properties,
                              vk::Buffer&             buffer,
                              vk::DeviceMemory&       buffer_memory)
{
    vk::BufferCreateInfo buffer_info {
        .flags       = {},
        .size        = size,
        .usage       = usage,
        .sharingMode = vk::SharingMode::eExclusive,
    };

    buffer = device.createBuffer(buffer_info);

    vk::MemoryRequirements memory_requirements = device.getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo alloc_info {
        .allocationSize  = memory_requirements.size,
        .memoryTypeIndex = FindMemoryType(physical_device, memory_requirements.memoryTypeBits, properties),
    };

    buffer_memory = device.allocateMemory(alloc_info);

    device.bindBufferMemory(buffer, buffer_memory, 0);
}

void VulkanUtil::CreateBufferAndInitialize(vk::Device              device,
                                           vk::PhysicalDevice      physicalDevice,
                                           vk::BufferUsageFlags    usageFlags,
                                           vk::MemoryPropertyFlags memoryPropertyFlags,
                                           vk::Buffer&              buffer,
                                           vk::DeviceMemory&        buffer_memory,
                                           vk::DeviceSize          size,
                                           void*                   data,
                                           int                     data_size)
{
    CreateBuffer(physicalDevice, device, size, usageFlags, memoryPropertyFlags, buffer, buffer_memory);
    
    if (!data || data_size == 0)
    {
        return;
    }

    void* mapped = device.mapMemory(buffer_memory, 0, size);
    memcpy(mapped, data, data_size);
    device.unmapMemory(buffer_memory);
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
        .flags         = create_flags,
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

void VulkanUtil::CreateGlobalImage(VulkanRHI*     rhi,
                                   vk::Image&     image,
                                   vk::ImageView& image_view,
                                   VmaAllocation& image_allocation,
                                   uint32_t       texture_width,
                                   uint32_t       texture_height,
                                   void*          texture_pixels,
                                   vk::Format     texture_format,
                                   uint32_t       miplevels)
{
    
    if (!texture_pixels)
    {
        return;
    }

    vk::DeviceSize texture_size = GetTextureSize(texture_width, texture_height, texture_format);

    LOG_ERROR("CreateGlobalImage not implement");
}

void VulkanUtil::CreateTextureImage(VulkanRHI*         rhi,
                                    vk::Image&         image,
                                    vk::ImageView&     image_view,
                                    vk::DeviceMemory&  image_memory,
                                    const TextureData* texture_data)
{
    if (!texture_data)
    {
        return;
    }

    vk::Device       device         = rhi->Device;
    const uint32_t&  texture_width  = texture_data->Width;
    const uint32_t&  texture_height = texture_data->Height;
    const vk::Format texture_format = texture_data->Format;

    vk::DeviceSize texture_size = GetTextureSize(texture_width, texture_height, texture_format);

    uint32_t mip_levels = (texture_data->MipLevels != 0) ? texture_data->MipLevels : floor(log2(std::max(texture_width, texture_height))) + 1;

    vk::Buffer       inefficient_staging_buffer;
    vk::DeviceMemory inefficient_staging_buffer_memory;

    CreateBuffer(rhi->PhysicalDevice,
                 rhi->Device,
                 texture_size,
                 vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                 inefficient_staging_buffer,
                 inefficient_staging_buffer_memory);

    void* data = device.mapMemory(inefficient_staging_buffer_memory, 0, texture_size);
    memcpy(data, texture_data->Pixels, (size_t)texture_size);
    device.unmapMemory(inefficient_staging_buffer_memory);

    CreateImage(rhi->PhysicalDevice,
                rhi->Device,
                texture_width,
                texture_height,
                texture_data->Format,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                image,
                image_memory,
                {},
                1,
                mip_levels,
                vk::SampleCountFlagBits::e1);

    TransitionImageLayout(rhi, image, texture_data->Format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor, 1, mip_levels);

    CopyBufferToImage(rhi, inefficient_staging_buffer, image, texture_width, texture_height);

    // TransitionImageLayout(texture_image,
    //                       texture_data->Format,
    //                       vk::ImageLayout::eTransferDstOptimal,
    //                       vk::ImageLayout::eShaderReadOnlyOptimal,
    //                       mip_levels);

    device.destroyBuffer(inefficient_staging_buffer);
    device.freeMemory(inefficient_staging_buffer_memory);

    GenerateTextureMipMaps(rhi, image, texture_data->Format, texture_width, texture_height, 1, mip_levels);

    image_view = CreateImageView(device, image, texture_data->Format, vk::ImageAspectFlagBits::eColor, vk::ImageViewType::e2D, 1, mip_levels);
}

void VulkanUtil::CreateTextureCube(VulkanRHI* rhi, vk::Image& image, vk::ImageView& image_view, vk::DeviceMemory& device_memory, std::array<const TextureData*, 6> textures_data)
{
    if (!textures_data[0])
    {
        return;
    }
    auto& texture_data = textures_data[0];

    vk::Device       device         = rhi->Device;
    const uint32_t&  texture_width  = texture_data->Width;
    const uint32_t&  texture_height = texture_data->Height;
    const vk::Format texture_format = texture_data->Format;

    vk::DeviceSize texture_layer_size = GetTextureSize(texture_width, texture_height, texture_format);
    vk::DeviceSize cube_texture_size = texture_layer_size * 6;

    uint32_t mip_levels = (texture_data->MipLevels != 0) ? texture_data->MipLevels : floor(log2(std::max(texture_width, texture_height))) + 1;

    vk::Buffer       inefficient_staging_buffer;
    vk::DeviceMemory inefficient_staging_buffer_memory;

    CreateBuffer(rhi->PhysicalDevice,
                 rhi->Device,
                 cube_texture_size,
                 vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                 inefficient_staging_buffer,
                 inefficient_staging_buffer_memory);

    void* data = device.mapMemory(inefficient_staging_buffer_memory, 0, cube_texture_size);
    {
        uint32_t cur_idx = 0;
        for (size_t i = 0; i < 6;)
        {
            memcpy(static_cast<byte*>(data) + texture_layer_size * i, textures_data[cur_idx]->Pixels, static_cast<size_t>(textures_data[cur_idx]->ArrayLayers * texture_layer_size));
            i += textures_data[cur_idx]->ArrayLayers;
            cur_idx++;
        }
    }
    device.unmapMemory(inefficient_staging_buffer_memory);

    CreateImage(rhi->PhysicalDevice,
                rhi->Device,
                texture_width,
                texture_height,
                texture_data->Format,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                image,
                device_memory,
                vk::ImageCreateFlagBits::eCubeCompatible,
                6,
                mip_levels,
                vk::SampleCountFlagBits::e1);

    TransitionImageLayout(rhi, image, texture_data->Format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor, 6, mip_levels);

    CopyBufferToImage(rhi, inefficient_staging_buffer, image, texture_width, texture_height, 6);

    // TransitionImageLayout(texture_image,
    //                       texture_data->Format,
    //                       vk::ImageLayout::eTransferDstOptimal,
    //                       vk::ImageLayout::eShaderReadOnlyOptimal,
    //                       mip_levels);

    device.destroyBuffer(inefficient_staging_buffer);
    device.freeMemory(inefficient_staging_buffer_memory);

    GenerateTextureMipMaps(rhi, image, texture_data->Format, texture_width, texture_height, 6, mip_levels);

    image_view = CreateImageView(device, image, texture_data->Format, vk::ImageAspectFlagBits::eColor, vk::ImageViewType::eCube, 6, mip_levels);
}

void VulkanUtil::GenerateTextureMipMaps(VulkanRHI* rhi,
                                        vk::Image  image,
                                        vk::Format image_format,
                                        uint32_t   texture_width,
                                        uint32_t   texture_height,
                                        uint32_t   layer_count,
                                        uint32_t   mip_levels)
{
    vk::FormatProperties format_properties = rhi->PhysicalDevice->getFormatProperties(image_format);
    if (!(format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    RHICommandBuffer* command_buffer = rhi->BeginSingleTimeCommands();

    vk::ImageSubresourceRange sub_resource_range {
        .aspectMask     = vk::ImageAspectFlagBits::eColor,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = layer_count,
    };

    vk::ImageMemoryBarrier barrier {
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = image,
        .subresourceRange    = sub_resource_range,
    };

    int32_t mip_width  = texture_width;
    int32_t mip_height = texture_height;
    for (size_t i = 1; i < mip_levels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout                     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask                 = vk::AccessFlagBits::eTransferRead;

        (*(VulkanCommandBuffer*)command_buffer)->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, {barrier});

        vk::ImageBlit blit;
        blit.srcOffsets[0]                 = vk::Offset3D {0, 0, 0};
        blit.srcOffsets[1]                 = vk::Offset3D {mip_width, mip_height, 1};
        blit.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        blit.srcSubresource.mipLevel       = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount     = layer_count;

        blit.dstOffsets[0]                 = vk::Offset3D {0, 0, 0};
        blit.dstOffsets[1]                 = vk::Offset3D {std::max(mip_width / 2, 1), std::max(mip_height / 2, 1), 1};
        blit.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        blit.dstSubresource.mipLevel       = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount     = layer_count;

        (*(VulkanCommandBuffer*)command_buffer)->blitImage(image,
                                 vk::ImageLayout::eTransferSrcOptimal,
                                 image,
                                 vk::ImageLayout::eTransferDstOptimal,
                                 {blit},
                                 vk::Filter::eLinear);

        barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        (*(VulkanCommandBuffer*)command_buffer)->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {barrier});

        mip_width  = std::max(mip_width / 2, 1);
        mip_height = std::max(mip_height / 2, 1);
    }

    barrier.subresourceRange.baseMipLevel = mip_levels - 1;
    barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout                     = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask                 = vk::AccessFlagBits::eShaderRead;

    (*(VulkanCommandBuffer*)command_buffer)->pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {barrier});

    rhi->EndSingleTimeCommands(command_buffer);
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
    RHICommandBuffer* command_buffer = rhi->BeginSingleTimeCommands();

    vk::ImageSubresourceRange sub_resource_range {
        .aspectMask     = aspect_mask_bits,
        .baseMipLevel   = 0,
        .levelCount     = mip_levels,
        .baseArrayLayer = 0,
        .layerCount     = layer_count,
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

    (*(VulkanCommandBuffer*)command_buffer)->pipelineBarrier(source_stage, destination_stage, {}, {}, {}, {barrier});

    rhi->EndSingleTimeCommands(command_buffer);
}

void VulkanUtil::CopyBuffer(VulkanRHI*     rhi,
                            vk::Buffer     srcBuffer,
                            vk::Buffer     dstBuffer,
                            vk::DeviceSize srcOffset,
                            vk::DeviceSize dstOffset,
                            vk::DeviceSize size)
{
    RHICommandBuffer* command_buffer = rhi->BeginSingleTimeCommands();

    vk::BufferCopy copy_region {
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
        .size      = size,
    };

    (*(VulkanCommandBuffer*)command_buffer)->copyBuffer(srcBuffer, dstBuffer, {copy_region});

    rhi->EndSingleTimeCommands(command_buffer);
}

void VulkanUtil::CopyBufferToImage(VulkanRHI* rhi,
                                   vk::Buffer buffer,
                                   vk::Image  image,
                                   uint32_t   width,
                                   uint32_t   height,
                                   uint32_t   layer_count)
{
    RHICommandBuffer* command_buffer = rhi->BeginSingleTimeCommands();

    vk::ImageSubresourceLayers image_subresource {
        .aspectMask     = vk::ImageAspectFlagBits::eColor,
        .mipLevel       = 0,
        .baseArrayLayer = 0,
        .layerCount     = layer_count,
    };

    vk::BufferImageCopy region {
        .bufferOffset      = 0,
        .bufferRowLength   = 0,
        .bufferImageHeight = 0,
        .imageSubresource  = image_subresource,
        .imageOffset       = {0, 0, 0},
        .imageExtent       = {width, height, 1},
    };

    (*(VulkanCommandBuffer*)command_buffer)->copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, {region});

    rhi->EndSingleTimeCommands(command_buffer);
}
