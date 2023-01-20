#include "render_system.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "function/render/forward_renderer.h"
#include "function/render/render_camera.h"
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


}

void RenderSystem::Clear() {}

void RenderSystem::Tick(float delta_time) {

}

void RenderSystem::ProcessSwapData() {}
