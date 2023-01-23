#pragma once

#include "render_pass.h"

class WindowUI;

enum ERenderPass
{

    Num
};

struct RenderPipelineInitInfo
{
};

class RenderPipeline
{
public:
    virtual ~RenderPipeline() {}

    virtual void Initialize(RenderPipelineInitInfo* init_info) = 0;
    virtual void InitializeUIRenderBackend(WindowUI* window_ui) {}
    virtual void Clear() {};

    virtual void PreparePassData()                  = 0;
    virtual void PassUpdateAfterRecreateSwapchain() = 0;


public:
    std::shared_ptr<VulkanRHI> RHI;
};
