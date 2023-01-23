#pragma once

#include "platform/platform.h"

#include <vulkan/vulkan.hpp>

class VulkanRHI;

class VulkanUtil
{
public:
    static uint32_t
    FindMemoryType(vk::PhysicalDevice physical_device, uint32_t type_filter, vk::MemoryPropertyFlags properties);

    static vk::SampleCountFlagBits GetMaxUsableSampleCount(vk::PhysicalDevice physical_device);

    static vk::Format FindSupportedFormat(vk::PhysicalDevice             physical_device,
                                          const std::vector<vk::Format>& candidates,
                                          vk::ImageTiling                tiling,
                                          vk::FormatFeatureFlags         features);

    static vk::Format FindDepthFormat(vk::PhysicalDevice physical_device);

    static bool HasStencilComponent(vk::Format format);

    static vk::DeviceSize GetTextureSize(uint32_t width, uint32_t height, vk::Format format);

    static vk::ShaderModule CreateShaderModule(vk::Device device, const std::vector<byte>& shader_code);

    static void CreateImage(vk::PhysicalDevice      physical_device,
                            vk::Device              device,
                            uint32_t                width,
                            uint32_t                height,
                            vk::Format              format,
                            vk::ImageTiling         tiling,
                            vk::ImageUsageFlags     usage,
                            vk::MemoryPropertyFlags properties,
                            vk::Image&              image,
                            vk::DeviceMemory&       image_memory,
                            vk::ImageCreateFlags    create_flags = {},
                            uint32_t                array_layers = 1,
                            uint32_t                mip_levels   = 1,
                            vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1);

    static vk::ImageView CreateImageView(vk::Device           device,
                                         vk::Image            image,
                                         vk::Format           format,
                                         vk::ImageAspectFlags aspect_flags = vk::ImageAspectFlagBits::eColor,
                                         vk::ImageViewType    view_type    = vk::ImageViewType::e2D,
                                         uint32_t             layout_count = 1,
                                         uint32_t             mip_levels   = 1);

    static void CreateGlobalImage(VulkanRHI*     rhi,
                                  vk::Image&     image,
                                  vk::ImageView& image_view,
                                  VmaAllocation& image_allocation,
                                  uint32_t       texture_width,
                                  uint32_t       texture_height,
                                  void*          texture_pixels,
                                  vk::Format     texture_format,
                                  uint32_t       miplevels);
    
    static void GenerateTextureMipMaps(VulkanRHI* rhi,
                                       vk::Image  image,
                                       vk::Format image_format,
                                       uint32_t   texture_width,
                                       uint32_t   texture_height,
                                       uint32_t   layer_count,
                                       uint32_t   mip_levels);

    static void TransitionImageLayout(VulkanRHI*           rhi,
                                      vk::Image            image,
                                      vk::Format           format,
                                      vk::ImageLayout      old_layout,
                                      vk::ImageLayout      new_layout,
                                      vk::ImageAspectFlags aspect_mask_bits = vk::ImageAspectFlagBits::eColor,
                                      uint32_t             layer_count      = 1,
                                      uint32_t             mip_levels       = 1);

};
