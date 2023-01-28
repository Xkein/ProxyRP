#pragma once

#include "function/render/render_pass.h"
#include "function/render/render_common.h"

struct DirectionalLightPassInitInfo : public RenderPassInitInfo
{
};

class DirectionalLightPass : public RenderPass
{
    static const uint32_t DirectionalLightShadowMapSize = 4096;

public:
    virtual void Initialize(const RenderPassInitInfo* init_info) override;
    virtual void PostInitialize() override;
    virtual void SetCommonInfo(RenderPassCommonInfo* common_info) override;
    virtual void PrepareData(RenderPassPrepareInfo* prepare_info) override;

    virtual void Draw() override;

private:
    void SetupAttachments();
    void SetupFramebuffer();
    void SetupDescriptorSetLayout();
    void SetupDescriptorSet();
    void SetupRenderPass();
    void SetupPipelines();

    std::shared_ptr<RenderPassCommon> PassCommon;

    std::vector<Descriptor> DescriptorInfos;
    std::vector<Pipeline>   RenderPipelines;

    MeshDirectionalLightShadowPerframeStorageBufferObject PerframeStorageBufferObject;
    VisiableNodes*                                        VisiableNodes;
};
