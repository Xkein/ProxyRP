#include "forward_renderer.h"
#include "function/render/render_pipeline.h"
#include "function/render/passes/render_pass_common.h"

void ForwardSceneRenderer::Render()
{
    Pipeline->PassCommon->ResetRingBuffer(RHI->GetCurrentFrameIndex());

    RHI->WaitForFences();

    RHI->ResetCommandPool();

    bool recreate_swapchain = RHI->PrepareBeforePass(std::bind(&RenderPipeline::PassUpdateAfterRecreateSwapchain, Pipeline.get()));
    if (recreate_swapchain)
        return;

    Pipeline->Draw();

    RHI->SubmitRendering(std::bind(&RenderPipeline::PassUpdateAfterRecreateSwapchain, Pipeline.get()));
}

void ForwardSceneRenderer::Clear() {}

void ForwardSceneRenderer::Initialize(RendererInitInfo* init_info)
{
    RHI             = init_info->RHI;
    Scene           = init_info->Scene;
    ResourceManager = init_info->ResourceManager;

    RenderPipelineInitInfo pipeline_init_info;
    pipeline_init_info.ResourceManager = init_info->ResourceManager;
    Pipeline      = std::make_shared<RenderPipeline>();
    Pipeline->RHI = RHI;
    Pipeline->Initialize(&pipeline_init_info);

}
