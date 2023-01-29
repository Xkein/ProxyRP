#include "rhi.h"

void RHI::CreateBuffer(RHIDeviceSize          size,
                             RHIBufferUsageFlags    usage,
                             RHIMemoryPropertyFlags properties,
                             RHIBufferRef&          buffer,
                             RHIDeviceMemoryRef&    buffer_memory)
{
    RHIBuffer*       vulkan_buffer;
    RHIDeviceMemory* vulkan_buffer_memory;
    CreateBuffer(size, usage, properties, vulkan_buffer, vulkan_buffer_memory);

    buffer        = RHIBufferRef(vulkan_buffer);
    buffer_memory = RHIDeviceMemoryRef(vulkan_buffer_memory);
}

void RHI::CreateBufferAndInitialize(RHIDeviceSize          size,
                                    RHIBufferUsageFlags    usage,
                                    RHIMemoryPropertyFlags properties,
                                    RHIBufferRef&          buffer,
                                    RHIDeviceMemoryRef&    buffer_memory,
                                    void*                  data,
                                    int                    data_size)
{
    RHIBuffer*       vulkan_buffer;
    RHIDeviceMemory* vulkan_buffer_memory;
    CreateBufferAndInitialize(size, usage, properties, vulkan_buffer, vulkan_buffer_memory, data, data_size);

    buffer        = RHIBufferRef(vulkan_buffer);
    buffer_memory = RHIDeviceMemoryRef(vulkan_buffer_memory);
}

void RHI::CopyBuffer(const RHIBufferRef& src_buffer, const RHIBufferRef& dst_buffer, RHIDeviceSize src_offset, RHIDeviceSize dst_offset, RHIDeviceSize size)
{
    CopyBuffer(src_buffer.get(), dst_buffer.get(), src_offset, dst_offset, size);
}

void RHI::CreateImage(uint32_t               width,
                      uint32_t               height,
                      RHIFormat              format,
                      RHIImageTiling         tiling,
                      RHIImageUsageFlags     usage,
                      RHIMemoryPropertyFlags properties,
                      RHIImageRef&           image,
                      RHIDeviceMemoryRef&    image_memory,
                      RHIImageCreateFlags    create_flags,
                      uint32_t               array_layers,
                      uint32_t               mip_levels,
                      RHISampleCountFlagBits sample_count)
{
    RHIImage*        vulkan_image;
    RHIDeviceMemory* vulkan_image_memory;
    CreateImage(width,
                height,
                format,
                tiling,
                usage,
                properties,
                vulkan_image,
                vulkan_image_memory,
                create_flags,
                array_layers,
                mip_levels,
                sample_count);

    image        = RHIImageRef(vulkan_image);
    image_memory = RHIDeviceMemoryRef(vulkan_image_memory);
}

void RHI::CreateImageView(const RHIImage*     image,
                          RHIFormat           format,
                          RHIImageAspectFlags image_aspect_flags,
                          RHIImageViewType    view_type,
                          uint32_t            layout_count,
                          uint32_t            miplevels,
                          RHIImageViewRef&    image_view)
{
    RHIImageView* vulkan_image_view;
    CreateImageView(image, format, image_aspect_flags, view_type, layout_count, miplevels, vulkan_image_view);

    image_view = RHIImageViewRef(vulkan_image_view);
}

void RHI::CreateTextureImage(RHIImageRef&        image,
                             RHIImageViewRef&    image_view,
                             RHIDeviceMemoryRef& image_memory,
                             const TextureData*  texure_data)
{
    RHIImage*        vulkan_image;
    RHIImageView*    vulkan_image_view;
    RHIDeviceMemory* vulkan_image_memory;
    CreateTextureImage(vulkan_image, vulkan_image_view, vulkan_image_memory, texure_data);

    image        = RHIImageRef(vulkan_image);
    image_view   = RHIImageViewRef(vulkan_image_view);
    image_memory = RHIDeviceMemoryRef(vulkan_image_memory);
}
