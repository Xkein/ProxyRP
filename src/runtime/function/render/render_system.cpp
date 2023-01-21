#include "render_system.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "function/render/render_camera.h"
#include "function/render/forward_renderer.h"
#include "function/render/deferred_renderer.h"
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

    Renderer = std::make_shared<ForwardRenderer>();
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

void RenderSystem::Tick(float delta_time) {
    SwapContext.SwapLogicRenderData();

    ProcessSwapData();
}

void RenderSystem::ProcessSwapData() {}
