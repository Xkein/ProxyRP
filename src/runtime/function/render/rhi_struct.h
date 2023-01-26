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

using RHIPipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo;

struct RHIDepthImageDesc
{
    RHIImage*     DepthImage     = RHI_NULL_HANDLE;
    RHIImageView* DepthImageView = RHI_NULL_HANDLE;
    RHIFormat     DepthImageFormat;
};
