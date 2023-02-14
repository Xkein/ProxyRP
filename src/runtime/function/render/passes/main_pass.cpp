#include "main_pass.h"
#include "function/render/passes/render_pass_common.h"
#include "function/render/render_scene.h"
#include "function/render/vulkan_rhi/vulkan_rhi_converter.h"
#include "function/render/render_mesh.h"

void MainRenderPass::Initialize(const RenderPassInitInfo* init_info)
{
    const MainPassInitInfo* main_pass_init_info = static_cast<const MainPassInitInfo*>(init_info);

    MeshPass        = main_pass_init_info->MeshPass;
    SkyboxPass      = main_pass_init_info->SkyboxPass;
    ToneMappingPass = main_pass_init_info->ToneMappingPass;

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

std::vector<RHIImageViewRef> MainRenderPass::GetFramebufferImageViews() const
{
    std::vector<RHIImageViewRef> image_views;
    for (const auto& attachment : Framebuffer.Attachments)
    {
        image_views.emplace_back(attachment.ImageViewRHI);
    }
    return image_views;
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

    RHI->PushEvent(RHI->GetCommandBuffer(), "Main Pass", {1.f, 1.f, 1.f, 1.f});

    RHI->BeginRenderPass(RHI->GetCommandBuffer(), &render_pass_begin_info, RHISubpassContents::eInline);


    MeshPass->Draw();
    SkyboxPass->Draw();

    RHI->NextSubpass(RHI->GetCommandBuffer(), RHISubpassContents::eInline);

    ToneMappingPass->Draw();

    RHI->EndRenderPass(RHI->GetCommandBuffer());

    RHI->PopEvent(RHI->GetCommandBuffer());
}

void MainRenderPass::SetupAttachments()
{
    auto swapchin_info = RHI->GetSwapchainInfo();

    Framebuffer.Attachments.resize(_pass_attachment_count);

    Framebuffer.Attachments[_pass_attachment_pass_buffer_b].Format = RHIFormat::eR16G16B16A16Sfloat;

    {
        Texture& pass_buffer_a_texture = Framebuffer.Attachments[_pass_attachment_pass_buffer_a];
        pass_buffer_a_texture.Format   = RHIFormat::eR16G16B16A16Sfloat;
        RHI->CreateImage(swapchin_info.Extent.width,
                         swapchin_info.Extent.height,
                         pass_buffer_a_texture.Format,
                         RHIImageTiling::eOptimal,
                         RHIImageUsageFlagBits::eColorAttachment | RHIImageUsageFlagBits::eInputAttachment | RHIImageUsageFlagBits::eTransientAttachment,
                         RHIMemoryPropertyFlagBits::eDeviceLocal,
                         pass_buffer_a_texture.ImageRHI,
                         pass_buffer_a_texture.DeviceMemoryRHI);
        RHI->CreateImageView(pass_buffer_a_texture.ImageRHI.get(), pass_buffer_a_texture.Format, RHIImageAspectFlagBits::eColor, RHIImageViewType::e2D, 1, 1, pass_buffer_a_texture.ImageViewRHI);

        Texture& pass_buffer_b_texture = Framebuffer.Attachments[_pass_attachment_pass_buffer_b];
        pass_buffer_b_texture.Format   = RHIFormat::eR16G16B16A16Sfloat;
        RHI->CreateImage(swapchin_info.Extent.width,
                         swapchin_info.Extent.height,
                         pass_buffer_b_texture.Format,
                         RHIImageTiling::eOptimal,
                         RHIImageUsageFlagBits::eColorAttachment | RHIImageUsageFlagBits::eInputAttachment | RHIImageUsageFlagBits::eTransientAttachment,
                         RHIMemoryPropertyFlagBits::eDeviceLocal,
                         pass_buffer_b_texture.ImageRHI,
                         pass_buffer_b_texture.DeviceMemoryRHI);
        RHI->CreateImageView(pass_buffer_b_texture.ImageRHI.get(), pass_buffer_b_texture.Format, RHIImageAspectFlagBits::eColor, RHIImageViewType::e2D, 1, 1, pass_buffer_b_texture.ImageViewRHI);
    }

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
            Framebuffer.Attachments[_pass_attachment_pass_buffer_a].ImageViewRHI.get(),
            Framebuffer.Attachments[_pass_attachment_pass_buffer_b].ImageViewRHI.get(),
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

    RHIAttachmentDescription& pass_buffer_a_attachment = attachments[_pass_attachment_pass_buffer_a];
    pass_buffer_a_attachment.format                    = Framebuffer.Attachments[_pass_attachment_pass_buffer_a].Format;
    pass_buffer_a_attachment.samples                   = RHISampleCountFlagBits::e1;
    pass_buffer_a_attachment.loadOp                    = RHIAttachmentLoadOp::eClear;
    pass_buffer_a_attachment.storeOp                   = RHIAttachmentStoreOp::eDontCare;
    pass_buffer_a_attachment.stencilLoadOp             = RHIAttachmentLoadOp::eDontCare;
    pass_buffer_a_attachment.stencilStoreOp            = RHIAttachmentStoreOp::eDontCare;
    pass_buffer_a_attachment.initialLayout             = RHIImageLayout::eUndefined;
    pass_buffer_a_attachment.finalLayout               = RHIImageLayout::eShaderReadOnlyOptimal;

    RHIAttachmentDescription& pass_buffer_b_attachment = attachments[_pass_attachment_pass_buffer_b];
    pass_buffer_b_attachment.format                    = Framebuffer.Attachments[_pass_attachment_pass_buffer_b].Format;
    pass_buffer_b_attachment.samples                   = RHISampleCountFlagBits::e1;
    pass_buffer_b_attachment.loadOp                    = RHIAttachmentLoadOp::eClear;
    pass_buffer_b_attachment.storeOp                   = RHIAttachmentStoreOp::eDontCare;
    pass_buffer_b_attachment.stencilLoadOp             = RHIAttachmentLoadOp::eDontCare;
    pass_buffer_b_attachment.stencilStoreOp            = RHIAttachmentStoreOp::eDontCare;
    pass_buffer_b_attachment.initialLayout             = RHIImageLayout::eUndefined;
    pass_buffer_b_attachment.finalLayout               = RHIImageLayout::eShaderReadOnlyOptimal;

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

    std::array<RHISubpassDescription, _subpass_count> subpasses {};

    std::vector<RHIAttachmentReference> attachment_references;
    std::vector<RHISubpassDependency> dependencies;

    // avoid address movement caused by reallocation
    attachment_references.reserve(_subpass_count * 3);
    dependencies.reserve(_subpass_count * 3);

    SetupRenderPass_Mesh(attachment_references, subpasses.data(), dependencies);
    SetupRenderPass_ToneMapping(attachment_references, subpasses.data(), dependencies);

    RHIRenderPassCreateInfo render_pass_create_info {};
    render_pass_create_info.attachmentCount = attachments.size();
    render_pass_create_info.pAttachments    = attachments.data();
    render_pass_create_info.subpassCount    = subpasses.size();
    render_pass_create_info.pSubpasses      = subpasses.data();
    render_pass_create_info.dependencyCount = dependencies.size();
    render_pass_create_info.pDependencies   = dependencies.data();

    Framebuffer.RenderPass = RHIRenderPassRef(RHI->CreateRenderPass(&render_pass_create_info));
}

void MainRenderPass::SetupRenderPass_Mesh(std::vector<RHIAttachmentReference>& attachment_references, RHISubpassDescription* subpasses, std::vector<RHISubpassDependency>& dependencies)
{
    RHIAttachmentReference& color_attachment_reference = attachment_references.emplace_back();
    color_attachment_reference.attachment              = _pass_attachment_pass_buffer_a;
    color_attachment_reference.layout                  = RHIImageLayout::eColorAttachmentOptimal;
    
    RHIAttachmentReference& depth_attachment_reference = attachment_references.emplace_back();
    depth_attachment_reference.attachment = _pass_attachment_depth;
    depth_attachment_reference.layout     = RHIImageLayout::eDepthStencilAttachmentOptimal;

    RHISubpassDescription& forward_lighting_pass  = subpasses[_subpass_forward_lighting];
    forward_lighting_pass.pipelineBindPoint       = RHIPipelineBindPoint::eGraphics;
    forward_lighting_pass.colorAttachmentCount    = 1;
    forward_lighting_pass.pColorAttachments       = &color_attachment_reference;
    forward_lighting_pass.pDepthStencilAttachment = &depth_attachment_reference;

    RHISubpassDependency& forward_lighting_pass_denpendency = dependencies.emplace_back();
    forward_lighting_pass_denpendency.srcSubpass            = VK_SUBPASS_EXTERNAL;
    forward_lighting_pass_denpendency.dstSubpass            = _subpass_forward_lighting;
    forward_lighting_pass_denpendency.srcStageMask          = RHIPipelineStageFlagBits::eColorAttachmentOutput;
    forward_lighting_pass_denpendency.dstStageMask          = RHIPipelineStageFlagBits::eFragmentShader | RHIPipelineStageFlagBits::eColorAttachmentOutput;
    forward_lighting_pass_denpendency.srcAccessMask         = {};
    forward_lighting_pass_denpendency.dstAccessMask         = RHIAccessFlagBits::eShaderRead | RHIAccessFlagBits::eColorAttachmentWrite;
}

void MainRenderPass::SetupRenderPass_ToneMapping(std::vector<RHIAttachmentReference>& attachment_references, RHISubpassDescription* subpasses, std::vector<RHISubpassDependency>& dependencies)
{
    RHIAttachmentReference& tone_mapping_input_attachment = attachment_references.emplace_back();
    tone_mapping_input_attachment.attachment = _pass_attachment_pass_buffer_a;
    tone_mapping_input_attachment.layout     = RHIImageLayout::eShaderReadOnlyOptimal;

    RHIAttachmentReference& tone_mapping_color_attachment = attachment_references.emplace_back();
    tone_mapping_color_attachment.attachment              = _pass_attachment_pass_buffer_b;
    tone_mapping_color_attachment.layout     = RHIImageLayout::eColorAttachmentOptimal;
    tone_mapping_color_attachment.attachment              = _pass_attachment_swap_chain_image;

    RHISubpassDescription& tone_mapping_pass  = subpasses[_subpass_tone_mapping];
    tone_mapping_pass.pipelineBindPoint       = RHIPipelineBindPoint::eGraphics;
    tone_mapping_pass.inputAttachmentCount    = 1;
    tone_mapping_pass.pInputAttachments       = &tone_mapping_input_attachment;
    tone_mapping_pass.colorAttachmentCount    = 1;
    tone_mapping_pass.pColorAttachments       = &tone_mapping_color_attachment;

    RHISubpassDependency& tone_mapping_pass_denpendency = dependencies.emplace_back();
    tone_mapping_pass_denpendency.srcSubpass            = _subpass_forward_lighting;
    tone_mapping_pass_denpendency.dstSubpass            = _subpass_tone_mapping;
    tone_mapping_pass_denpendency.srcStageMask          = RHIPipelineStageFlagBits::eFragmentShader | RHIPipelineStageFlagBits::eColorAttachmentOutput;
    tone_mapping_pass_denpendency.dstStageMask          = RHIPipelineStageFlagBits::eFragmentShader | RHIPipelineStageFlagBits::eColorAttachmentOutput;
    tone_mapping_pass_denpendency.srcAccessMask         = RHIAccessFlagBits::eShaderWrite| RHIAccessFlagBits::eColorAttachmentWrite;
    tone_mapping_pass_denpendency.dstAccessMask         = RHIAccessFlagBits::eShaderRead | RHIAccessFlagBits::eColorAttachmentRead;
    tone_mapping_pass_denpendency.dependencyFlags       = RHIDependencyFlagBits::eByRegion;
}
