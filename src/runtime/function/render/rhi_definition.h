#pragma once

#include <vulkan/vulkan.hpp>

#define RHI_SUBPASS_EXTERNAL VK_SUBPASS_EXTERNAL
#define RHI_NULL_HANDLE VK_NULL_HANDLE

using RHIFormat = vk::Format;
using RHIImageTiling = vk::ImageTiling;
using RHIImageUsageFlags = vk::ImageUsageFlags;
using RHIImageUsageFlagBits = vk::ImageUsageFlagBits;
using RHIImageCreateFlags = vk::ImageCreateFlags;
using RHIImageCreateFlagBits = vk::ImageCreateFlagBits;
using RHIImageAspectFlags = vk::ImageAspectFlags;
using RHIImageAspectFlagBits = vk::ImageAspectFlagBits;
using RHIImageViewType = vk::ImageViewType;
using RHIImageLayout = vk::ImageLayout;

using RHIMemoryPropertyFlags = vk::MemoryPropertyFlags;
using RHIMemoryPropertyFlagBits = vk::MemoryPropertyFlagBits;

using RHISampleCountFlagBits = vk::SampleCountFlagBits;

using RHIAttachmentDescriptionFlags = vk::AttachmentDescriptionFlags;
using RHIAttachmentDescriptionFlagBits = vk::AttachmentDescriptionFlagBits;

using RHIAttachmentLoadOp = vk::AttachmentLoadOp;
using RHIAttachmentStoreOp = vk::AttachmentStoreOp;

using RHIPipelineBindPoint = vk::PipelineBindPoint;
using RHIPipelineStageFlags = vk::PipelineStageFlags;
using RHIPipelineStageFlagBits = vk::PipelineStageFlagBits;

using RHIDescriptorType = vk::DescriptorType;

using RHIShaderStageFlags = vk::ShaderStageFlags;
using RHIShaderStageFlagBits = vk::ShaderStageFlagBits;
