#include "directional_light_pass.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "render_pass_common.h"
#include "function/render/shader.h"
#include "function/render/shader_map.h"
#include "function/render/render_mesh.h"
#include "function/render/render_scene.h"
#include "platform/file/file_manager.h"

class DirectionalLightShadowVS : public Shader
{
public:
    DECLARE_SHADER(DirectionalLightShadowVS);

};
class DirectionalLightShadowPS : public Shader
{
public:
    DECLARE_SHADER(DirectionalLightShadowPS);

};

IMPLEMENT_SHADER_TYPE(DirectionalLightShadowVS, "directional_light_shadow.hlsl", "vert", SF_Vertex);
IMPLEMENT_SHADER_TYPE(DirectionalLightShadowPS, "directional_light_shadow.hlsl", "frag", SF_Pixel);

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
    PerframeStorageBufferObject.LightProjView = prepare_info->Scene->LightProjView;
    VisiableNodes                             = &prepare_info->Scene->VisiableNodes;
}

void DirectionalLightPass::Draw()
{
    struct MeshNode
    {
        const Matrix4x4* model_matrix {nullptr};
        const Matrix4x4* joint_matrices {nullptr};
        uint32_t         joint_count {0};
    };


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
    
    std::array<vk::ImageView, 2> vk_attachments {*(VulkanImageView*)attachments[0], *(VulkanImageView*)attachments[1]};
    RHIFramebufferCreateInfo     create_info {
            .renderPass      = *(VulkanRenderPass*)Framebuffer.RenderPass.get(),
            .attachmentCount = attachments.size(),
            .pAttachments    = vk_attachments.data(),
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
    RHIDescriptorSetAllocateInfo descriptor_set_alloc_info {
        .descriptorPool     = *(VulkanDescriptorPool*)RHI->GetDescriptorPool(),
        .descriptorSetCount = 1,
        .pSetLayouts        = (vk::DescriptorSetLayout*)DescriptorInfos[0].LayoutRHI.get(),
    };

    DescriptorInfos[0].DescriptorSetRHI = RHIDescriptorSetRef(RHI->AllocateDescriptorSets(&descriptor_set_alloc_info));

    RHIDescriptorBufferInfo perframe_storage_buffer_info {
        .buffer = *(VulkanBuffer*)PassCommon->GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffer.BufferRHI.get(),
        .offset = 0,
        .range  = sizeof(MeshDirectionalLightShadowPerframeStorageBufferObject),
    };

    RHIDescriptorBufferInfo perdrawcall_storage_buffer_info {
        .buffer = *(VulkanBuffer*)PassCommon->GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffer.BufferRHI.get(),
        .offset = 0,
        .range  = sizeof(MeshDirectionalLightShadowPerdrawcallStorageBufferObject),
    };

    RHIDescriptorBufferInfo perdrawcall_vertex_blending_storage_buffer_info {
        .buffer = *(VulkanBuffer*)PassCommon->GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffer.BufferRHI.get(),
        .offset = 0,
        .range  = sizeof(MeshDirectionalLightShadowPerdrawcallVertexBlendingStorageBufferObject),
    };

    RHIDescriptorSet* descriptor_set = DescriptorInfos[0].DescriptorSetRHI.get();
    
    std::array<RHIWriteDescriptorSet, 3> descriptor_writes {};

    RHIWriteDescriptorSet& perframe_storage_buffer_write = descriptor_writes[0];
    perframe_storage_buffer_write.dstSet                 = (vk::DescriptorSet)*(VulkanDescriptorSet*)descriptor_set;
    perframe_storage_buffer_write.dstBinding             = 0;
    perframe_storage_buffer_write.dstArrayElement        = 0;
    perframe_storage_buffer_write.descriptorType         = RHIDescriptorType::eStorageBufferDynamic;
    perframe_storage_buffer_write.descriptorCount        = 1;
    perframe_storage_buffer_write.pBufferInfo            = &perframe_storage_buffer_info;

    RHIWriteDescriptorSet& perdrawcall_storage_buffer_write = descriptor_writes[1];
    perdrawcall_storage_buffer_write.dstSet                 = (vk::DescriptorSet)*(VulkanDescriptorSet*)descriptor_set;
    perdrawcall_storage_buffer_write.dstBinding             = 1;
    perdrawcall_storage_buffer_write.dstArrayElement        = 0;
    perdrawcall_storage_buffer_write.descriptorType         = RHIDescriptorType::eStorageBufferDynamic;
    perdrawcall_storage_buffer_write.descriptorCount        = 1;
    perdrawcall_storage_buffer_write.pBufferInfo            = &perdrawcall_storage_buffer_info;

    RHIWriteDescriptorSet& perdrawcall_vertex_blending_storage_buffer_write = descriptor_writes[1];
    perdrawcall_vertex_blending_storage_buffer_write.dstSet                 = (vk::DescriptorSet)*(VulkanDescriptorSet*)descriptor_set;
    perdrawcall_vertex_blending_storage_buffer_write.dstBinding             = 2;
    perdrawcall_vertex_blending_storage_buffer_write.dstArrayElement        = 0;
    perdrawcall_vertex_blending_storage_buffer_write.descriptorType         = RHIDescriptorType::eStorageBufferDynamic;
    perdrawcall_vertex_blending_storage_buffer_write.descriptorCount        = 1;
    perdrawcall_vertex_blending_storage_buffer_write.pBufferInfo            = &perdrawcall_vertex_blending_storage_buffer_info;

    RHI->UpdateDescriptorSets(descriptor_writes, {});
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

    std::array<RHIDescriptorSetLayout*, 2> descriptor_layouts {DescriptorInfos[0].LayoutRHI.get(),
                                                               PassCommon->PerMeshLayout.get()};
    std::array<vk::DescriptorSetLayout, 2> vk_descriptor_layouts {*(VulkanDescriptorSetLayout*)descriptor_layouts[0],
                                                                  *(VulkanDescriptorSetLayout*)descriptor_layouts[1]};
    RHIPipelineLayoutCreateInfo            pipeline_layout_create_info {
                   .setLayoutCount = descriptor_layouts.size(),
                   .pSetLayouts    = vk_descriptor_layouts.data(),
    };

    RenderPipelines[0].LayoutRHI = RHIPipelineLayoutRef(RHI->CreatePipelineLayout(&pipeline_layout_create_info));

    RHIShader* vert_shader_module =
        RHI->CreateShaderModule(FileManager::Read(DirectionalLightShadowVS::StaticType.SourceFile));
    RHIShader* frag_shader_module =
        RHI->CreateShaderModule(FileManager::Read(DirectionalLightShadowPS::StaticType.SourceFile));

    RHIPipelineShaderStageCreateInfo vert_pipeline_shader_stage_create_info {
        .stage  = RHIShaderStageFlagBits::eVertex,
        .module = *(VulkanShader*)vert_shader_module,
        .pName  = DirectionalLightShadowPS::StaticType.EntryPoint.c_str(),
    };
    RHIPipelineShaderStageCreateInfo frag_pipeline_shader_stage_create_info {
        .stage  = RHIShaderStageFlagBits::eFragment,
        .module = *(VulkanShader*)frag_shader_module,
        .pName  = DirectionalLightShadowPS::StaticType.EntryPoint.c_str(),
    };

    std::array<RHIPipelineShaderStageCreateInfo, 2> shader_stages {vert_pipeline_shader_stage_create_info,
                                                                   frag_pipeline_shader_stage_create_info};


    auto vertex_binding_descriptions   = MeshVertex::GetBindingDescriptions();
    auto vertex_attribute_descriptions = MeshVertex::GetAttributeDescription();

    RHIPipelineVertexInputStateCreateInfo vertex_input_state_create_info {
        .vertexBindingDescriptionCount   = 1,
        .pVertexBindingDescriptions      = &vertex_binding_descriptions[0],
        .vertexAttributeDescriptionCount = 1,
        .pVertexAttributeDescriptions    = &vertex_attribute_descriptions[0],
    };

    RHIPipelineInputAssemblyStateCreateInfo input_assembly_create_info {
        .topology               = RHIPrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = RHI_FALSE,
    };

    RHIViewport viewport {
        .x        = 0,
        .y        = 0,
        .width    = DirectionalLightShadowMapSize,
        .height   = DirectionalLightShadowMapSize,
        .minDepth = 0.0,
        .maxDepth = 1.0,
    };
    RHIRect2D scissor {.offset = {0, 0}, .extent = {DirectionalLightShadowMapSize, DirectionalLightShadowMapSize}};

    RHIPipelineViewportStateCreateInfo viewport_state_create_info {
        .viewportCount = 1,
        .pViewports    = &viewport,
        .scissorCount  = 1,
        .pScissors     = &scissor,
    };

    RHIPipelineRasterizationStateCreateInfo rasterization_state_create_info {
        .depthClampEnable        = RHI_FALSE,
        .rasterizerDiscardEnable = RHI_FALSE,
        .polygonMode             = RHIPolygonMode::eFill,
        .cullMode                = RHICullModeFlagBits::eBack,
        .frontFace               = RHIFrontFace::eCounterClockwise,
        .depthBiasEnable         = RHI_FALSE,
        .lineWidth               = 1.f,
    };

    RHIPipelineMultisampleStateCreateInfo multisample_state_create_info {
        .rasterizationSamples = RHISampleCountFlagBits::e1,
        .sampleShadingEnable  = RHI_FALSE,
    };

    RHIPipelineColorBlendAttachmentState color_blend_attachment_state {
        .blendEnable         = RHI_FALSE,
        .srcColorBlendFactor = RHIBlendFactor::eOne,
        .dstColorBlendFactor = RHIBlendFactor::eZero,
        .colorBlendOp        = RHIBlendOp::eAdd,
        .srcAlphaBlendFactor = RHIBlendFactor::eOne,
        .dstAlphaBlendFactor = RHIBlendFactor::eZero,
        .alphaBlendOp        = RHIBlendOp::eAdd,
    };

    RHIPipelineColorBlendStateCreateInfo color_blend_state_create_info {
        .logicOpEnable   = RHI_FALSE,
        .attachmentCount = 1,
        .pAttachments    = &color_blend_attachment_state,
    };

    RHIPipelineDepthStencilStateCreateInfo depth_stencil_create_info {
        .depthTestEnable       = RHI_TRUE,
        .depthWriteEnable      = RHI_TRUE,
        .depthCompareOp        = RHICompareOp::eLess,
        .depthBoundsTestEnable = RHI_FALSE,
        .stencilTestEnable     = RHI_FALSE,
    };

    RHIPipelineDynamicStateCreateInfo dynamic_state_create_info {};

    RHIGraphicsPipelineCreateInfo pipeline_info {
        .stageCount          = shader_stages.size(),
        .pStages             = shader_stages.data(),
        .pVertexInputState   = &vertex_input_state_create_info,
        .pInputAssemblyState = &input_assembly_create_info,
        .pViewportState      = &viewport_state_create_info,
        .pRasterizationState = &rasterization_state_create_info,
        .pMultisampleState   = &multisample_state_create_info,
        .pDepthStencilState  = &depth_stencil_create_info,
        .pColorBlendState    = &color_blend_state_create_info,
        .pDynamicState       = &dynamic_state_create_info,
        .layout              = *(VulkanPipelineLayout*)RenderPipelines[0].LayoutRHI.get(),
        .renderPass          = *(VulkanRenderPass*)Framebuffer.RenderPass.get(),
        .subpass             = 0,
    };

    RenderPipelines[0].PipelineRHI = RHIPipelineRef(RHI->CreateGraphicsPipeline(RHI_NULL_HANDLE, &pipeline_info));

    RHI->DestroyShaderModule(vert_shader_module);
    RHI->DestroyShaderModule(frag_shader_module);
}
