#include "render_system.h"

#include "function/render/vulkan_rhi/vulkan_rhi.h"

RenderSystem::~RenderSystem() {}

void RenderSystem::Initialize(RenderSystemInitInfo init_info)
{
    RHI = std::make_shared<VulkanRHI>();
    RHIInitInfo rhi_init_info;
    rhi_init_info.WindowSystem = init_info.WindowSystem;
    RHI->Initialize(rhi_init_info);
}

void RenderSystem::Clear() {}

void RenderSystem::Tick(float delta_time) {

}

void RenderSystem::ProcessSwapData() {}
