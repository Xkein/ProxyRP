#pragma once

#include "function/render/render_common.h"
#include "function/render/render_pass.h"

struct VisiableNodes;

struct PointLightPassInitInfo : public RenderPassInitInfo
{
};

class PointLightRenderPass : public RenderPass
{
    static const uint32_t PointLightShadowMapSize = 2048;

public:
    virtual void Initialize(const RenderPassInitInfo* init_info) override;
    virtual void PostInitialize() override;
    virtual void PrepareData(RenderPassPrepareInfo* prepare_info) override;
    virtual void UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info) override {};

    virtual void Draw() override;

    RHIImageViewRef GetPointLightShadowMap() const;

private:
    void SetupAttachments();
    void SetupFramebuffer();
    void SetupDescriptorSetLayout();
    void SetupDescriptorSet();
    void SetupRenderPass();
    void SetupPipelines();

    MeshPointLightShadowPerframeStorageBufferObject PerframeStorageBufferObject;
    VisiableNodes*                                        VisiableNodes;
};
