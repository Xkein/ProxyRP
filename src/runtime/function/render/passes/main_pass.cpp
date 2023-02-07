#include "main_pass.h"
#include "function/render/passes/render_pass_common.h"
#include "function/render/render_scene.h"
#include "function/render/vulkan_rhi/vulkan_rhi_converter.h"
#include "function/render/render_mesh.h"

void MainRenderPass::Initialize(const RenderPassInitInfo* init_info)
{
    const MainPassInitInfo* main_pass_init_info = static_cast<const MainPassInitInfo*>(init_info);

    MeshPass = main_pass_init_info->MeshPass;

    SetupAttachments();
    SetupRenderPass();
    SetupFramebuffer();
    SetupFramebufferDescriptorSet();
}

void MainRenderPass::PostInitialize() {}

void MainRenderPass::PrepareData(RenderPassPrepareInfo* prepare_info) {}

RHIRenderPassRef MainRenderPass::GetRenderPass() const
{
    return Framebuffer.RenderPass;
}

void MainRenderPass::UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info)
{
    for (size_t i = 0; i < Framebuffer.Attachments.size(); i++)
    {
        RHI->DestroyImage(Framebuffer.Attachments[i].ImageRHI.get());
        RHI->DestroyImageView(Framebuffer.Attachments[i].ImageViewRHI.get());
        RHI->FreeMemory(Framebuffer.Attachments[i].DeviceMemoryRHI.get());
    }

    for (auto& Framebuffer : SwapchainFramebuffers)
    {
        RHI->DestroyFramebuffer(Framebuffer.get());
    }

    SetupAttachments();
    SetupFramebuffer();
    SetupFramebufferDescriptorSet();
}

void MainRenderPass::Draw()
{
    auto swapchain_info = RHI->GetSwapchainInfo();

    std::array<RHIClearValue, _pass_attachment_count> clear_values {};
    clear_values[_pass_attachment_depth].depthStencil = RHIClearDepthStencilValue {1.f, 0};
    clear_values[_pass_attachment_swap_chain_image].color.setFloat32({0.f});

    RHIRenderPassBeginInfo render_pass_begin_info {
        .renderPass      = VulkanRHIConverter::Convert(*Framebuffer.RenderPass),
        .framebuffer     = VulkanRHIConverter::Convert(*SwapchainFramebuffers[RHI->GetCurrentSwapchainIndex()]),
        .renderArea      = {0, 0, swapchain_info.Extent},
        .clearValueCount = clear_values.size(),
        .pClearValues    = clear_values.data(),
    };

    RHI->BeginRenderPass(RHI->GetCommandBuffer(), &render_pass_begin_info, RHISubpassContents::eInline);

    RHI->PushEvent(RHI->GetCommandBuffer(), "Main Pass", {1.f, 1.f, 1.f, 1.f});

    MeshPass->Draw();

    RHI->PopEvent(RHI->GetCommandBuffer());

    RHI->EndRenderPass(RHI->GetCommandBuffer());
}

void MainRenderPass::SetupAttachments()
{
    Framebuffer.Attachments.resize(_pass_attachment_count);

    Framebuffer.Attachments[_pass_attachment_depth].Format = RHI->GetDepthImageInfo().DepthImageFormat;

    Framebuffer.Attachments[_pass_attachment_swap_chain_image].Format = RHI->GetSwapchainInfo().ImageFormat;
}

void MainRenderPass::SetupFramebuffer()
{
    auto swapchin_info = RHI->GetSwapchainInfo();

    SwapchainFramebuffers.resize(swapchin_info.ImageViews.size());

    for (size_t i = 0; i < swapchin_info.ImageViews.size(); i++)
    {
        std::array<RHIImageView*, _pass_attachment_count> attachments {
            RHI->GetDepthImageInfo().DepthImageView,
            swapchin_info.ImageViews[i],
        };

        auto                     vk_attachments = VulkanRHIConverter::Convert(attachments);
        RHIFramebufferCreateInfo framebuffer_create_info {
            .renderPass      = VulkanRHIConverter::Convert(*Framebuffer.RenderPass),
            .attachmentCount = attachments.size(),
            .pAttachments    = vk_attachments.data(),
            .width           = swapchin_info.Extent.width,
            .height          = swapchin_info.Extent.height,
            .layers          = 1,
        };

        SwapchainFramebuffers[i] = RHIFramebufferRef(RHI->CreateFramebuffer(&framebuffer_create_info));
    }
}

void MainRenderPass::SetupFramebufferDescriptorSet() {}

void MainRenderPass::SetupRenderPass()
{
    std::array<RHIAttachmentDescription, _pass_attachment_count> attachments {};

    RHIAttachmentDescription& color_attachment = attachments[_pass_attachment_swap_chain_image];
    color_attachment.format                    = Framebuffer.Attachments[_pass_attachment_swap_chain_image].Format;
    color_attachment.samples                   = RHI->GetMsaaSampleCount();
    color_attachment.loadOp                    = RHIAttachmentLoadOp::eClear;
    color_attachment.storeOp                   = RHIAttachmentStoreOp::eStore;
    color_attachment.stencilLoadOp             = RHIAttachmentLoadOp::eDontCare;
    color_attachment.stencilStoreOp            = RHIAttachmentStoreOp::eDontCare;
    color_attachment.initialLayout             = RHIImageLayout::eUndefined;
    color_attachment.finalLayout               = RHIImageLayout::ePresentSrcKHR;

    RHIAttachmentDescription& depth_attachment = attachments[_pass_attachment_depth];
    depth_attachment.format                    = Framebuffer.Attachments[_pass_attachment_depth].Format;
    depth_attachment.samples                   = RHI->GetMsaaSampleCount();
    depth_attachment.loadOp                    = RHIAttachmentLoadOp::eClear;
    depth_attachment.storeOp                   = RHIAttachmentStoreOp::eDontCare;
    depth_attachment.stencilLoadOp             = RHIAttachmentLoadOp::eDontCare;
    depth_attachment.stencilStoreOp            = RHIAttachmentStoreOp::eDontCare;
    depth_attachment.initialLayout             = RHIImageLayout::eUndefined;
    depth_attachment.finalLayout               = RHIImageLayout::eDepthStencilAttachmentOptimal;

    RHIAttachmentReference color_attachment_reference {
        .attachment = _pass_attachment_swap_chain_image,
        .layout     = RHIImageLayout::eColorAttachmentOptimal,
    };
    RHIAttachmentReference depth_attachment_reference {
        .attachment = _pass_attachment_depth,
        .layout     = RHIImageLayout::eDepthStencilAttachmentOptimal,
    };

    std::array<RHISubpassDescription, _subpass_count> subpasses {};

    RHISubpassDescription& forward_lighting_pass  = subpasses[_subpass_forward_lighting];
    forward_lighting_pass.pipelineBindPoint       = RHIPipelineBindPoint::eGraphics;
    forward_lighting_pass.colorAttachmentCount    = 1;
    forward_lighting_pass.pColorAttachments       = &color_attachment_reference;
    forward_lighting_pass.pDepthStencilAttachment = &depth_attachment_reference;

    std::array<RHISubpassDependency, 1> dependencies {};

    RHISubpassDependency& forward_lighting_pass_denpendency = dependencies[0];
    forward_lighting_pass_denpendency.srcSubpass            = VK_SUBPASS_EXTERNAL;
    forward_lighting_pass_denpendency.dstSubpass            = _subpass_forward_lighting;
    forward_lighting_pass_denpendency.srcStageMask          = RHIPipelineStageFlagBits::eColorAttachmentOutput;
    forward_lighting_pass_denpendency.dstStageMask          = RHIPipelineStageFlagBits::eFragmentShader | RHIPipelineStageFlagBits::eColorAttachmentOutput;
    forward_lighting_pass_denpendency.srcAccessMask         = {};
    forward_lighting_pass_denpendency.dstAccessMask         = RHIAccessFlagBits::eShaderRead | RHIAccessFlagBits::eColorAttachmentWrite;

    RHIRenderPassCreateInfo render_pass_create_info {};
    render_pass_create_info.attachmentCount = attachments.size();
    render_pass_create_info.pAttachments    = attachments.data();
    render_pass_create_info.subpassCount    = subpasses.size();
    render_pass_create_info.pSubpasses      = subpasses.data();
    render_pass_create_info.dependencyCount = dependencies.size();
    render_pass_create_info.pDependencies   = dependencies.data();

    Framebuffer.RenderPass = RHIRenderPassRef(RHI->CreateRenderPass(&render_pass_create_info));
}
