#pragma once

#include <vulkan/vulkan.hpp>

#define RHI_SUBPASS_EXTERNAL VK_SUBPASS_EXTERNAL
#define RHI_WHOLE_SIZE (~0ull) 
#define RHI_NULL_HANDLE nullptr
#define RHI_TRUE true
#define RHI_FALSE false

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
using RHIFilter = vk::Filter;
using RHISamplerAddressMode = vk::SamplerAddressMode;
using RHISamplerMipmapMode = vk::SamplerMipmapMode;

using RHIBorderColor = vk::BorderColor;

using RHIBufferUsageFlags = vk::BufferUsageFlags;
using RHIBufferUsageFlagBits = vk::BufferUsageFlagBits;
using RHICommandBufferUsageFlags = vk::CommandBufferUsageFlags;
using RHICommandBufferUsageFlagBits = vk::CommandBufferUsageFlagBits;

using RHIMemoryPropertyFlags = vk::MemoryPropertyFlags;
using RHIMemoryPropertyFlagBits = vk::MemoryPropertyFlagBits;

using RHISampleCountFlagBits = vk::SampleCountFlagBits;

using RHIAttachmentDescriptionFlags = vk::AttachmentDescriptionFlags;
using RHIAttachmentDescriptionFlagBits = vk::AttachmentDescriptionFlagBits;

using RHIAttachmentLoadOp = vk::AttachmentLoadOp;
using RHIAttachmentStoreOp = vk::AttachmentStoreOp;

using RHICommandBufferLevel = vk::CommandBufferLevel;

using RHISubpassContents = vk::SubpassContents;

using RHIAccessFlagBits = vk::AccessFlagBits;

using RHIDependencyFlags = vk::DependencyFlags;
using RHIDependencyFlagBits = vk::DependencyFlagBits;

using RHIPipelineBindPoint = vk::PipelineBindPoint;
using RHIPipelineStageFlags = vk::PipelineStageFlags;
using RHIPipelineStageFlagBits = vk::PipelineStageFlagBits;

using RHIDescriptorType = vk::DescriptorType;

using RHIShaderStageFlags = vk::ShaderStageFlags;
using RHIShaderStageFlagBits = vk::ShaderStageFlagBits;

using RHIVertexInputRate = vk::VertexInputRate;

using RHIPrimitiveTopology = vk::PrimitiveTopology;
using RHIPolygonMode = vk::PolygonMode;
using RHICullModeFlags = vk::CullModeFlags;
using RHICullModeFlagBits = vk::CullModeFlagBits;
using RHIFrontFace = vk::FrontFace;
using RHIBlendOp = vk::BlendOp;
using RHIBlendFactor = vk::BlendFactor;
using RHILogicOp = vk::LogicOp;
using RHICompareOp = vk::CompareOp;

using RHIDeviceSize = vk::DeviceSize;
using RHIIndexType = vk::IndexType;



enum class RHIDefaultSamplerType
{
    Linear,
    Nearest
};
