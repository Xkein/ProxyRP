#pragma once

#include "rhi.h"
#include "function/render/render_resource.h"
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

struct FramebufferRecreateInfo
{
    RHIImageViewRef InputAttachment;

};


class RenderPass
{
public:
    virtual void Initialize(const RenderPassInitInfo* init_info) = 0;
    virtual void PostInitialize() {}
    virtual void SetCommonInfo(RenderPassCommonInfo common_info);
    virtual void PrepareData() = 0;
    virtual void UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info) = 0;

    virtual void Draw() = 0;

public:
    struct Framebuffer
    {
        int               Width;
        int               Height;
        RHIFramebufferRef Framebuffer;
        RHIRenderPassRef  Render_pass;
        
        std::vector<TextureRef> Attachments;
    };

protected:
    std::shared_ptr<VulkanRHI> RHI;

    Framebuffer Framebuffer;
};
