#pragma once

#include "rhi.h"
#include "vulkan/vulkan.hpp"

#include <vector>

class VulkanRHI;
class WindowUI;

struct RenderPassInitInfo
{};

struct RenderPassCommonInfo
{
    std::shared_ptr<VulkanRHI> RHI;
};


class RenderPass
{
public:
    virtual void Initialize(const RenderPassInitInfo* init_info) = 0;
    virtual void PostInitialize() {}
    virtual void SetCommonInfo(RenderPassCommonInfo common_info);
    virtual void PrepareData() = 0;
    virtual void UpdateAfterFramebufferRecreate() = 0;

    virtual void Draw() = 0;

public:
    struct FrameBufferAttachment
    {
        vk::Image*        Image;
        vk::DeviceMemory* Mem;
        vk::ImageView*    View;
        vk::Format        Format;
    };

    struct Framebuffer
    {
        int              Width;
        int              Height;
        vk::Framebuffer* Framebuffer;
        vk::RenderPass*  Render_pass;

        std::vector<FrameBufferAttachment> Attachments;
    };

    struct Descriptor
    {
        vk::DescriptorSetLayout* Layout;
        vk::DescriptorSet*       Descriptor_set;
    };

    struct RenderPipelineBase
    {
        vk::PipelineLayout* Layout;
        vk::Pipeline*       Pipeline;
    };

protected:
    std::shared_ptr<VulkanRHI> RHI;

    std::vector<Descriptor>         DescriptorInfos;
    std::vector<RenderPipelineBase> RenderPipelines;
    Framebuffer                     Framebuffer;
};
