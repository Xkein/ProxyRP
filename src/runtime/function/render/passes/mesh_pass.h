#pragma once

#include "function/render/render_pass.h"

struct MeshPassInitInfo : public RenderPassInitInfo
{

};

class MeshPass : public RenderPass
{
public:
    virtual void Initialize(const RenderPassInitInfo* init_info) override;
    virtual void PrepareData() override;
    virtual void UpdateAfterFramebufferRecreate() override;
    virtual void Draw() override;

private:
    void SetupAttachments();
    void SetupDescriptorSetLayout();
    void SetupDescriptorSet();
    void SetupFramebufferDescriptorSet();
    void SetupPipelines();

    std::vector<vk::Framebuffer> SwapchainFrameBuffers;
};

