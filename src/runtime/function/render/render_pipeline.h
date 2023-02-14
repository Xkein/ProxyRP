#pragma once

#include "render_pass.h"

class WindowUI;

enum ERenderPass
{

    Num
};

struct RenderPipelineInitInfo
{
    std::shared_ptr<RenderResourceManager> ResourceManager;
};

struct RenderPipelinePrepareInfo
{
    std::shared_ptr<RenderScene>           Scene;
    std::shared_ptr<RenderResourceManager> ResourceManager;
};

class RenderPipeline
{
public:
    virtual ~RenderPipeline() {}

    virtual void Initialize(RenderPipelineInitInfo* init_info);
    virtual void InitializeUIRenderBackend(WindowUI* window_ui) {}
    virtual void Clear() {};

    virtual void PreparePassData(RenderPipelinePrepareInfo* prepare_info);
    virtual void PassUpdateAfterRecreateSwapchain();

    virtual void Draw();


public:
    std::shared_ptr<RHI> RHI;

    std::shared_ptr<RenderPassCommon> PassCommon;

    std::shared_ptr<RenderPass> DirectionalLightPass;
    std::shared_ptr<RenderPass> PointLightShadowPass;
    std::shared_ptr<RenderPass> MainPass;

    std::shared_ptr<RenderPass> MeshPass;
    std::shared_ptr<RenderPass> SkyboxPass;
    std::shared_ptr<RenderPass> FxaaPass;
    std::shared_ptr<RenderPass> ColorGradingPass;
    std::shared_ptr<RenderPass> ToneMappingPass;
    std::shared_ptr<RenderPass> UIPass;
};
