#pragma once

#include "function/render/render_pipeline.h"

class RenderPassCommon;

class ForwardPipeline : public RenderPipeline
{
public:
    virtual void Initialize(RenderPipelineInitInfo* init_info) override;
    virtual void PreparePassData(RenderPipelinePrepareInfo* prepare_info) override;
    virtual void PassUpdateAfterRecreateSwapchain() override;

public:

    std::shared_ptr<RenderPass> DirectionalLightPass;
    std::shared_ptr<RenderPass> PointLightShadowPass;
    std::shared_ptr<RenderPass> MeshPass;

    std::shared_ptr<RenderPass> FxaaPass;
    std::shared_ptr<RenderPass> ColorGradingPass;
    std::shared_ptr<RenderPass> ToneMappingPass;
    std::shared_ptr<RenderPass> UIPass;
};
