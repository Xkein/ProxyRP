#pragma once

#include "rhi_resource.h"
#include "rhi_definition.h"


using RHIAttachmentDescription =  vk::AttachmentDescription;
using RHIAttachmentReference =  vk::AttachmentReference;

using RHISubpassDescription = vk::SubpassDescription;
using RHISubpassDependency = vk::SubpassDependency;

using RHIRenderPassCreateInfo = vk::RenderPassCreateInfo;

using RHIFramebufferCreateInfo = vk::FramebufferCreateInfo;

using RHIDescriptorSetLayoutBinding = vk::DescriptorSetLayoutBinding;
using RHIDescriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo;
using RHIDescriptorSetAllocateInfo = vk::DescriptorSetAllocateInfo;
using RHIDescriptorBufferInfo = vk::DescriptorBufferInfo;
using RHIWriteDescriptorSet = vk::WriteDescriptorSet;
using RHICopyDescriptorSet = vk::CopyDescriptorSet;
using RHIDescriptorImageInfo = vk::DescriptorImageInfo;

using RHIPipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo;

using RHIPipelineShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo;
using RHIPipelineVertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo;
using RHIPipelineInputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo;
using RHIPipelineViewportStateCreateInfo = vk::PipelineViewportStateCreateInfo;
using RHIPipelineRasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo;
using RHIPipelineMultisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo;
using RHIPipelineColorBlendAttachmentState = vk::PipelineColorBlendAttachmentState;
using RHIPipelineColorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo;
using RHIPipelineDepthStencilStateCreateInfo = vk::PipelineDepthStencilStateCreateInfo;
using RHIPipelineDynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo;
using RHIGraphicsPipelineCreateInfo = vk::GraphicsPipelineCreateInfo;

using RHIDynamicState = vk::DynamicState;

using RHIVertexInputBindingDescription = vk::VertexInputBindingDescription;
using RHIVertexInputAttributeDescription = vk::VertexInputAttributeDescription;

using RHIExtent2D = vk::Extent2D;
using RHIViewport = vk::Viewport;
using RHIRect2D = vk::Rect2D;

struct RHISwapchainDesc
{
    RHIFormat    ImageFormat;
    RHIExtent2D  Extent;
    RHIViewport* Viewport;
    RHIRect2D*   Scissor;

    std::vector<RHIImageView*> ImageViews;
};

struct RHIDepthImageDesc
{
    RHIImage*     DepthImage     = RHI_NULL_HANDLE;
    RHIImageView* DepthImageView = RHI_NULL_HANDLE;
    RHIFormat     DepthImageFormat;
};
