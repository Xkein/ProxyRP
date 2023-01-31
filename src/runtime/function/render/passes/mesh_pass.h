#pragma once

#include "function/render/render_pass.h"

struct MeshPassInitInfo : public RenderPassInitInfo
{
    RHIImageViewRef DirectionalLightShadowMap;
};

class MeshPass : public RenderPass
{
public:
    virtual void Initialize(const RenderPassInitInfo* init_info) override;
    virtual void PostInitialize() override;
    virtual void SetCommonInfo(RenderPassCommonInfo* common_info) override;
    virtual void PrepareData(RenderPassPrepareInfo* prepare_info) override;
    virtual void UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info) override;
    virtual void Draw() override;

private:
    void SetupAttachments();
    void SetupFramebuffer();
    void SetupDescriptorSetLayout();
    void SetupDescriptorSet();
    void SetupRenderPass();
    void SetupPipelines();


};

