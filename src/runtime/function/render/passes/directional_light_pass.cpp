#include "directional_light_pass.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "render_pass_common.h"
#include "function/render/shader.h"
#include "function/render/shader_map.h"

class DirectionalLightShadowVS : public Shader
{

};

void DirectionalLightPass::Initialize(const RenderPassInitInfo* init_info)
{
    SetupAttachments();
    SetupRenderPass();
    SetupFramebuffer();
    SetupDescriptorSetLayout();
}

void DirectionalLightPass::PostInitialize()
{
    SetupPipelines();
    SetupDescriptorSet();
}

void DirectionalLightPass::SetCommonInfo(RenderPassCommonInfo* common_info)
{
    RenderPass::SetCommonInfo(common_info);

    PassCommon = common_info->PassCommon;
}

void DirectionalLightPass::PrepareData(RenderPassPrepareInfo* prepare_info)
{
}

void DirectionalLightPass::UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info)
{
}

void DirectionalLightPass::Draw()
{
}

void DirectionalLightPass::SetupAttachments()
{
    // color and depth
    Framebuffer.Attachments.resize(2);

    Texture& color_texture            = Framebuffer.Attachments[0];
    color_texture.Format   = RHIFormat::eR32Sfloat;
    RHI->CreateImage(DirectionalLightShadowMapSize,
                     DirectionalLightShadowMapSize,
                     color_texture.Format,
                     RHIImageTiling::eOptimal,
                     RHIImageUsageFlagBits::eColorAttachment | RHIImageUsageFlagBits::eSampled,
                     RHIMemoryPropertyFlagBits::eDeviceLocal,
                     color_texture.ImageRHI,
                     color_texture.DeviceMemoryRHI);
    RHI->CreateImageView(color_texture.ImageRHI.get(),
                         color_texture.Format,
                         RHIImageAspectFlagBits::eColor,
                         RHIImageViewType::e2D,
                         1,
                         1,
                         color_texture.ImageViewRHI);

    Texture& depth_texture            = Framebuffer.Attachments[1];
    depth_texture.Format   = RHIFormat::eR32Sfloat;
    RHI->CreateImage(DirectionalLightShadowMapSize,
                     DirectionalLightShadowMapSize,
                     depth_texture.Format,
                     RHIImageTiling::eOptimal,
                     RHIImageUsageFlagBits::eDepthStencilAttachment | RHIImageUsageFlagBits::eTransientAttachment,
                     RHIMemoryPropertyFlagBits::eDeviceLocal,
                     depth_texture.ImageRHI,
                     depth_texture.DeviceMemoryRHI);
    RHI->CreateImageView(depth_texture.ImageRHI.get(),
                         depth_texture.Format,
                         RHIImageAspectFlagBits::eDepth,
                         RHIImageViewType::e2D,
                         1,
                         1,
                         depth_texture.ImageViewRHI);
}

void DirectionalLightPass::SetupFramebuffer()
{
    std::array<RHIImageView*, 2> attachments {Framebuffer.Attachments[0].ImageViewRHI.get(),
                                              Framebuffer.Attachments[1].ImageViewRHI.get()};

    RHIFramebufferCreateInfo create_info {
        .renderPass      = *(VulkanRenderPass*)Framebuffer.RenderPass.get(),
        .attachmentCount = attachments.size(),
        .pAttachments    = (vk::ImageView*)attachments.data(),
        .width           = DirectionalLightShadowMapSize,
        .height          = DirectionalLightShadowMapSize,
        .layers          = 1,
    };

    Framebuffer.Framebuffer = RHIFramebufferRef(RHI->CreateFramebuffer(&create_info));
}

void DirectionalLightPass::SetupDescriptorSetLayout()
{
    DescriptorInfos.resize(1);

    std::array<RHIDescriptorSetLayoutBinding, 3> layout_bindings;

    RHIDescriptorSetLayoutBinding& perframe_storage_buffer_binding = layout_bindings[0];
    perframe_storage_buffer_binding.binding                        = 0;
    perframe_storage_buffer_binding.descriptorType                 = RHIDescriptorType::eStorageBufferDynamic;
    perframe_storage_buffer_binding.descriptorCount                = 1;
    perframe_storage_buffer_binding.stageFlags                     = RHIShaderStageFlagBits::eVertex;

    RHIDescriptorSetLayoutBinding& perdrawcall_storage_buffer_binding = layout_bindings[1];
    perdrawcall_storage_buffer_binding.binding                        = 1;
    perdrawcall_storage_buffer_binding.descriptorType                 = RHIDescriptorType::eStorageBufferDynamic;
    perdrawcall_storage_buffer_binding.descriptorCount                = 1;
    perdrawcall_storage_buffer_binding.stageFlags                     = RHIShaderStageFlagBits::eVertex;

    RHIDescriptorSetLayoutBinding& vertex_blending_storage_buffer_binding = layout_bindings[2];
    vertex_blending_storage_buffer_binding.binding                        = 2;
    vertex_blending_storage_buffer_binding.descriptorType                 = RHIDescriptorType::eStorageBufferDynamic;
    vertex_blending_storage_buffer_binding.descriptorCount                = 1;
    vertex_blending_storage_buffer_binding.stageFlags                     = RHIShaderStageFlagBits::eVertex;

    RHIDescriptorSetLayoutCreateInfo layout_create_info {
        .bindingCount = layout_bindings.size(),
        .pBindings    = layout_bindings.data(),
    };

    DescriptorInfos[0].LayoutRHI = RHIDescriptorSetLayoutRef(RHI->CreateDescriptorSetLayout(&layout_create_info));
}

void DirectionalLightPass::SetupDescriptorSet()
{
}

void DirectionalLightPass::SetupRenderPass()
{
    std::array<RHIAttachmentDescription, 2> attachments {};

    RHIAttachmentDescription& color_attachment = attachments[0];
    color_attachment.format                    = Framebuffer.Attachments[0].Format;
    color_attachment.samples                   = RHISampleCountFlagBits::e1;
    color_attachment.loadOp                    = RHIAttachmentLoadOp::eClear;
    color_attachment.storeOp                   = RHIAttachmentStoreOp::eStore;
    color_attachment.stencilLoadOp             = RHIAttachmentLoadOp::eDontCare;
    color_attachment.stencilStoreOp            = RHIAttachmentStoreOp::eDontCare;
    color_attachment.initialLayout             = RHIImageLayout::eUndefined;
    color_attachment.finalLayout               = RHIImageLayout::eShaderReadOnlyOptimal;

    RHIAttachmentDescription& depth_attachment = attachments[0];
    depth_attachment.format                    = Framebuffer.Attachments[0].Format;
    depth_attachment.samples                   = RHISampleCountFlagBits::e1;
    depth_attachment.loadOp                    = RHIAttachmentLoadOp::eClear;
    depth_attachment.storeOp                   = RHIAttachmentStoreOp::eDontCare;
    depth_attachment.stencilLoadOp             = RHIAttachmentLoadOp::eDontCare;
    depth_attachment.stencilStoreOp            = RHIAttachmentStoreOp::eDontCare;
    depth_attachment.initialLayout             = RHIImageLayout::eUndefined;
    depth_attachment.finalLayout               = RHIImageLayout::eDepthStencilAttachmentOptimal;

    RHIAttachmentReference color_attachment_reference {
        .attachment = 0,
        .layout     = RHIImageLayout::eColorAttachmentOptimal,
    };
    RHIAttachmentReference depth_attachment_reference {
        .attachment = 1,
        .layout     = RHIImageLayout::eDepthStencilAttachmentOptimal,
    };

    std::array<RHISubpassDescription, 1> subpasses {};
    RHISubpassDescription& shadow_pass = subpasses[0];
    shadow_pass.pipelineBindPoint      = RHIPipelineBindPoint::eGraphics;
    shadow_pass.colorAttachmentCount   = 1;
    shadow_pass.pColorAttachments      = &color_attachment_reference;
    shadow_pass.pDepthStencilAttachment = &depth_attachment_reference;

    std::array<RHISubpassDependency, 1> dependencies {};
    RHISubpassDependency& lighting_pass_denpendency = dependencies[0];
    lighting_pass_denpendency.srcSubpass            = 0;
    lighting_pass_denpendency.dstSubpass            = RHI_SUBPASS_EXTERNAL;
    lighting_pass_denpendency.srcStageMask          = RHIPipelineStageFlagBits::eColorAttachmentOutput;
    lighting_pass_denpendency.dstStageMask          = RHIPipelineStageFlagBits::eBottomOfPipe;

    RHIRenderPassCreateInfo render_pass_create_info {};
    render_pass_create_info.attachmentCount = attachments.size();
    render_pass_create_info.pAttachments    = attachments.data();
    render_pass_create_info.subpassCount    = subpasses.size();
    render_pass_create_info.pSubpasses      = subpasses.data();
    render_pass_create_info.dependencyCount = dependencies.size();
    render_pass_create_info.pDependencies   = dependencies.data();

    Framebuffer.RenderPass = RHIRenderPassRef(RHI->CreateRenderPass(&render_pass_create_info));
}

void DirectionalLightPass::SetupPipelines()
{
    RenderPipelines.resize(1);

    std::array<RHIDescriptorSetLayout*, 2> descriptor_layouts {DescriptorInfos[0].LayoutRHI.get(), PassCommon->PerMeshLayout.get()};
    std::array<vk::DescriptorSetLayout, 2> vk_descriptor_layouts {*(VulkanDescriptorSetLayout*)descriptor_layouts[0], *(VulkanDescriptorSetLayout*)descriptor_layouts[1]};
    RHIPipelineLayoutCreateInfo            pipeline_layout_create_info {
                   .setLayoutCount = descriptor_layouts.size(),
                   .pSetLayouts    = vk_descriptor_layouts.data(),
    };
    
    RenderPipelines[0].LayoutRHI = RHIPipelineLayoutRef(RHI->CreatePipelineLayout(&pipeline_layout_create_info));


}
