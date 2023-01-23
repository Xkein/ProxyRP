#include "render_system.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "function/render/render_camera.h"
#include "function/render/render_scene.h"
#include "function/render/pipeline/forward_pipeline.h"
#include "function/render/renderer/forward_renderer.h"
#include "function/global/global_context.h"
#include "resource/asset/asset_manager.h"
#include "resource/config/config_manager.h"
#include "resource/res_type/global/global_rendering.h"

RenderSystem::~RenderSystem() {}

void RenderSystem::Initialize(RenderSystemInitInfo init_info)
{
    RHIInitInfo rhi_init_info;
    rhi_init_info.WindowSystem = init_info.WindowSystem;

    RHI = std::make_shared<VulkanRHI>();
    RHI->Initialize(rhi_init_info);

    GlobalRenderingResource global_rendering_res;
    GAssetManager->LoadAsset(GConfigManager->Global.GlobalRenderingResourceUrl, global_rendering_res);

    Camera = std::make_shared<RenderCamera>();

    Scene = std::make_shared<RenderScene>();

    Renderer = std::make_shared<ForwardSceneRenderer>();
    Renderer->RHI    = RHI;
    Renderer->Scene  = Scene;

    RenderPipelineInitInfo pipeline_init_info;
    Pipeline = std::static_pointer_cast<RenderPipeline>(std::make_shared<ForwardPipeline>());
    Pipeline->RHI = RHI;
    Pipeline->Initialize(&pipeline_init_info);

}

void RenderSystem::Clear()
{
    if (RHI)
    {
        RHI->Clear();
    }
    RHI.reset();

    if (Renderer)
    {
        Renderer->Clear();
    }
    Renderer.reset();
}

void RenderSystem::Tick(float delta_time)
{
    SwapLogicRenderData();

    ProcessSwapData();

    RHI->PrepareContext();

    Renderer->UpdatePerBuffer(Camera);

    Scene->UpdateVisibleObjects(Camera);

    Pipeline->PreparePassData();

    Renderer->Render();
}

void RenderSystem::SwapLogicRenderData()
{
    SwapContext.SwapLogicRenderData();
}

RenderSwapContext& RenderSystem::GetSwapContext()
{
    return SwapContext;
}

std::shared_ptr<VulkanRHI> RenderSystem::GetRHI() const
{
    return RHI;
}

std::shared_ptr<RenderCamera> RenderSystem::GetRenderCamera() const
{
    return Camera;
}

void RenderSystem::ProcessSwapData()
{
    RenderSwapData& swap_data = SwapContext.GetRenderSwapData();

    std::shared_ptr<AssetManager> asset_manager = GAssetManager;

}
