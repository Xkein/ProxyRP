#pragma once

#include "function/render/render_common.h"
#include "function/render/render_pass.h"
#include "function/render/render_type.h"

struct SkyboxPassInitInfo : public RenderPassInitInfo
{
    RHIRenderPassRef RenderPass;
};

class SkyboxRenderPass : public RenderPass
{
public:
    virtual void Initialize(const RenderPassInitInfo* init_info) override;
    virtual void PostInitialize() override;
    virtual void PrepareData(RenderPassPrepareInfo* prepare_info) override;
    virtual void UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info) override;
    virtual void Draw() override;

private:
    void SetupAttachments();
    void SetupFramebufferDescriptorSet();
    void SetupDescriptorSetLayout();
    void SetupDescriptorSet();
    void SetupPipelines();

    MeshPerframeStorageBufferObject PerframeStorageBufferObject;
};
