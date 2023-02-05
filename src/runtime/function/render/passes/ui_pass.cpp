#include "ui_pass.h"
#include "function/ui/window_ui.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "function/render/vulkan_rhi/vulkan_rhi_converter.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

void UIPass::Initialize(const RenderPassInitInfo* init_info)
{
    const UIPassInitInfo* ui_init_info = static_cast<const UIPassInitInfo*>(init_info);
    Framebuffer.RenderPass             = ui_init_info->Pass;
}

void UIPass::InitializeUIRenderBackend(WindowUI* window_ui)
{
    UI = window_ui;

    VulkanRHI* vk_rhi = (VulkanRHI*)RHI.get();
    ImGui_ImplGlfw_InitForVulkan(vk_rhi->Window, true);
    ImGui_ImplVulkan_InitInfo init_info {
        .Instance = vk_rhi->Instance,
        .PhysicalDevice = vk_rhi->PhysicalDevice,
        .Device = vk_rhi->Device,
        .QueueFamily    = vk_rhi->QueueIndices.GraphicsFamily.value(),
        .Queue = vk_rhi->GraphicsQueue,
        .DescriptorPool = *vk_rhi->DescriptorPool,
        .Subpass = 114514,
        .MinImageCount = 3,
        .ImageCount = 3
    };

    ImGui_ImplVulkan_Init(&init_info, *(VulkanRenderPass*)Framebuffer.RenderPass.get());

    UploadFonts();
}

void UIPass::Draw()
{
    if (!UI)
        return;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    UI->PreRender();

    RHI->PushEvent(RHI->GetCommandBuffer(), "ImGUI", {1.0f, 1.0f, 1.0f, 1.0f});

    ImGui::Render();

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VulkanRHIConverter::Convert(*RHI->GetCommandBuffer()));

    RHI->PopEvent(RHI->GetCommandBuffer());
}

void UIPass::UploadFonts()
{
    RHICommandBuffer* command_buffer = RHI->BeginSingleTimeCommands();

    ImGui_ImplVulkan_CreateFontsTexture(VulkanRHIConverter::Convert(*command_buffer));

    RHI->EndSingleTimeCommands(command_buffer);

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}
