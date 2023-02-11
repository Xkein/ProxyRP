#include "skybox_pass.h"
#include "function/render/shader.h"
#include "function/render/vulkan_rhi/vulkan_rhi_converter.h"
#include "function/render/passes/render_pass_common.h"
#include "platform/file/file_manager.h"
#include "function/render/render_mesh.h"
#include "function/render/render_scene.h"

class SkyboxVS : public Shader
{
public:
    DECLARE_SHADER(SkyboxVS);
};
class SkyboxPS : public Shader
{
public:
    DECLARE_SHADER(SkyboxPS);
};

IMPLEMENT_SHADER_TYPE(SkyboxVS, "skybox.hlsl", "vert", SF_Vertex);
IMPLEMENT_SHADER_TYPE(SkyboxPS, "skybox.hlsl", "frag", SF_Pixel);

void SkyboxRenderPass::Initialize(const RenderPassInitInfo* init_info)
{
    const SkyboxPassInitInfo* skybox_pass_init_info = static_cast<const SkyboxPassInitInfo*>(init_info);

    Framebuffer.RenderPass = skybox_pass_init_info->RenderPass;

    SetupDescriptorSetLayout();
    SetupAttachments();
    SetupPipelines();
    SetupDescriptorSet();
    SetupFramebufferDescriptorSet();
}

void SkyboxRenderPass::PostInitialize() {}

void SkyboxRenderPass::PrepareData(RenderPassPrepareInfo* prepare_info)
{
    PerframeStorageBufferObject = prepare_info->Scene->PerframeStorageBufferObject;
}

void SkyboxRenderPass::UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info) {}

void SkyboxRenderPass::Draw()
{
    auto swapchain_info = RHI->GetSwapchainInfo();

    uint32_t                         perframe_dynamic_offset;
    MeshPerframeStorageBufferObject* perframe_storage_buffer_object =
        PassCommon->GlobalRenderResource._StorageBuffers.AllocateFromRingbuffer<MeshPerframeStorageBufferObject>(RHI->GetCurrentFrameIndex(), perframe_dynamic_offset);
    *perframe_storage_buffer_object = PerframeStorageBufferObject;

    RHI->PushEvent(RHI->GetCommandBuffer(), "Skybox", {1.f, 1.f, 1.f, 1.f});

    RHI->BindPipeline(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[0].PipelineRHI.get());
    RHI->SetViewport(RHI->GetCommandBuffer(), 0, std::array {*swapchain_info.Viewport});
    RHI->SetScissor(RHI->GetCommandBuffer(), 0, std::array {*swapchain_info.Scissor});
    const RHIDescriptorSet* skybox_descriptor_sets[] {DescriptorInfos[0].DescriptorSetRHI.get()};
    std::array<uint32_t, 1> dynamic_offsets {perframe_dynamic_offset};
    RHI->BindDescriptorSets(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[0].LayoutRHI.get(), 0, skybox_descriptor_sets, dynamic_offsets);

    RHI->Draw(RHI->GetCommandBuffer(), 36, 1, 0, 0);

    RHI->PopEvent(RHI->GetCommandBuffer());
}

void SkyboxRenderPass::SetupAttachments()
{
    Framebuffer.Attachments.resize(_pass_attachment_count);

    Framebuffer.Attachments[_pass_attachment_depth].Format = RHI->GetDepthImageInfo().DepthImageFormat;

    Framebuffer.Attachments[_pass_attachment_swap_chain_image].Format = RHI->GetSwapchainInfo().ImageFormat;
}

void SkyboxRenderPass::SetupFramebufferDescriptorSet() {}

void SkyboxRenderPass::SetupDescriptorSetLayout()
{
    DescriptorInfos.resize(1);

    std::array<RHIDescriptorSetLayoutBinding, 2> mesh_global_layout_bindings;

    RHIDescriptorSetLayoutBinding& perframe_storage_buffer_binding = mesh_global_layout_bindings[0];
    perframe_storage_buffer_binding.binding                        = 0;
    perframe_storage_buffer_binding.descriptorType                 = RHIDescriptorType::eStorageBufferDynamic;
    perframe_storage_buffer_binding.descriptorCount                = 1;
    perframe_storage_buffer_binding.stageFlags                     = RHIShaderStageFlagBits::eVertex;

    RHIDescriptorSetLayoutBinding& skybox_texture_layout = mesh_global_layout_bindings[1];
    skybox_texture_layout.binding                        = 1;
    skybox_texture_layout.descriptorType                 = RHIDescriptorType::eCombinedImageSampler;
    skybox_texture_layout.descriptorCount                = 1;
    skybox_texture_layout.stageFlags                     = RHIShaderStageFlagBits::eFragment;

    RHIDescriptorSetLayoutCreateInfo layout_create_info {
        .bindingCount = mesh_global_layout_bindings.size(),
        .pBindings    = mesh_global_layout_bindings.data(),
    };

    DescriptorInfos[0].LayoutRHI = RHIDescriptorSetLayoutRef(RHI->CreateDescriptorSetLayout(&layout_create_info));
}

void SkyboxRenderPass::SetupDescriptorSet()
{
    RHIDescriptorSetAllocateInfo mesh_global_descriptor_set_alloc_info {
        .descriptorPool     = VulkanRHIConverter::Convert(*RHI->GetDescriptorPool()),
        .descriptorSetCount = 1,
        .pSetLayouts        = (vk::DescriptorSetLayout*)DescriptorInfos[0].LayoutRHI.get(),
    };

    DescriptorInfos[0].DescriptorSetRHI = RHIDescriptorSetRef(RHI->AllocateDescriptorSets(&mesh_global_descriptor_set_alloc_info));

    RHIDescriptorBufferInfo perframe_storage_buffer_info {
        .buffer = VulkanRHIConverter::Convert(*PassCommon->GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffer.BufferRHI),
        .offset = 0,
        .range  = sizeof(MeshPerframeStorageBufferObject),
    };
    ASSERT(perframe_storage_buffer_info.range < PassCommon->GlobalRenderResource._StorageBuffers.MaxStorageBufferRange);

    RHIDescriptorImageInfo specular_texture_image_info {};
    specular_texture_image_info.sampler     = VulkanRHIConverter::Convert(*PassCommon->GlobalRenderResource._IBLResource.SpecularTextureSampler);
    specular_texture_image_info.imageView   = VulkanRHIConverter::Convert(*PassCommon->GlobalRenderResource._IBLResource.SpecularTexture.ImageViewRHI);
    specular_texture_image_info.imageLayout = RHIImageLayout::eShaderReadOnlyOptimal;

    RHIDescriptorSet* mesh_globaldescriptor_set = DescriptorInfos[0].DescriptorSetRHI.get();
    
    std::array<RHIWriteDescriptorSet, 2> descriptor_writes {};

    RHIWriteDescriptorSet& perframe_storage_buffer_write = descriptor_writes[0];
    perframe_storage_buffer_write.dstSet                 = VulkanRHIConverter::Convert(*mesh_globaldescriptor_set);
    perframe_storage_buffer_write.dstBinding             = 0;
    perframe_storage_buffer_write.dstArrayElement        = 0;
    perframe_storage_buffer_write.descriptorType         = RHIDescriptorType::eStorageBufferDynamic;
    perframe_storage_buffer_write.descriptorCount        = 1;
    perframe_storage_buffer_write.pBufferInfo            = &perframe_storage_buffer_info;

    RHIWriteDescriptorSet& directional_light_shadow_texture_image_write = descriptor_writes[1];
    directional_light_shadow_texture_image_write.dstSet                 = VulkanRHIConverter::Convert(*mesh_globaldescriptor_set);
    directional_light_shadow_texture_image_write.dstBinding             = 1;
    directional_light_shadow_texture_image_write.dstArrayElement        = 0;
    directional_light_shadow_texture_image_write.descriptorType         = RHIDescriptorType::eCombinedImageSampler;
    directional_light_shadow_texture_image_write.descriptorCount        = 1;
    directional_light_shadow_texture_image_write.pImageInfo             = &specular_texture_image_info;

    RHI->UpdateDescriptorSets(descriptor_writes, {});
}

void SkyboxRenderPass::SetupPipelines()
{
    auto swapchain_info = RHI->GetSwapchainInfo();

    RenderPipelines.resize(1);

    std::array<RHIDescriptorSetLayout*, 1> descriptor_layouts {DescriptorInfos[0].LayoutRHI.get()};
    auto                        vk_descriptor_layouts = VulkanRHIConverter::Convert(descriptor_layouts);
    RHIPipelineLayoutCreateInfo pipeline_layout_create_info {
        .setLayoutCount = descriptor_layouts.size(),
        .pSetLayouts    = vk_descriptor_layouts.data(),
    };

    RenderPipelines[0].LayoutRHI = RHIPipelineLayoutRef(RHI->CreatePipelineLayout(&pipeline_layout_create_info));

    RHIShader* vert_shader_module = RHI->CreateShaderModule(FileManager::Read(SkyboxVS::StaticType.GetCachedFilePath()));
    RHIShader* frag_shader_module = RHI->CreateShaderModule(FileManager::Read(SkyboxPS::StaticType.GetCachedFilePath()));

    RHIPipelineShaderStageCreateInfo vert_pipeline_shader_stage_create_info {
        .stage  = RHIShaderStageFlagBits::eVertex,
        .module = *(VulkanShader*)vert_shader_module,
        .pName  = SkyboxVS::StaticType.EntryPoint.c_str(),
    };
    RHIPipelineShaderStageCreateInfo frag_pipeline_shader_stage_create_info {
        .stage  = RHIShaderStageFlagBits::eFragment,
        .module = *(VulkanShader*)frag_shader_module,
        .pName  = SkyboxPS::StaticType.EntryPoint.c_str(),
    };

    std::array<RHIPipelineShaderStageCreateInfo, 2> shader_stages {vert_pipeline_shader_stage_create_info, frag_pipeline_shader_stage_create_info};

    auto vertex_binding_descriptions   = MeshVertex::GetBindingDescriptions();
    auto vertex_attribute_descriptions = MeshVertex::GetAttributeDescription();

    RHIPipelineVertexInputStateCreateInfo vertex_input_state_create_info {
        .vertexBindingDescriptionCount   = vertex_binding_descriptions.size(),
        .pVertexBindingDescriptions      = vertex_binding_descriptions.data(),
        .vertexAttributeDescriptionCount = vertex_attribute_descriptions.size(),
        .pVertexAttributeDescriptions    = vertex_attribute_descriptions.data(),
    };
    vertex_input_state_create_info.vertexBindingDescriptionCount   = 0;
    vertex_input_state_create_info.pVertexBindingDescriptions      = nullptr;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
    vertex_input_state_create_info.pVertexAttributeDescriptions    = nullptr;

    RHIPipelineInputAssemblyStateCreateInfo input_assembly_create_info {
        .topology               = RHIPrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = RHI_FALSE,
    };

    RHIPipelineViewportStateCreateInfo viewport_state_create_info {
        .viewportCount = 1,
        .pViewports    = swapchain_info.Viewport,
        .scissorCount  = 1,
        .pScissors     = swapchain_info.Scissor,
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
        .colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
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

    std::array<vk::DynamicState, 2> dynamic_states {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    RHIPipelineDynamicStateCreateInfo dynamic_state_create_info {
        .dynamicStateCount = (uint32_t)dynamic_states.size(),
        .pDynamicStates    = dynamic_states.data(),
    };

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
        .subpass             = _subpass_forward_lighting,
    };

    RenderPipelines[0].PipelineRHI = RHIPipelineRef(RHI->CreateGraphicsPipeline(RHI_NULL_HANDLE, &pipeline_info));

    RHI->DestroyShaderModule(vert_shader_module);
    RHI->DestroyShaderModule(frag_shader_module);
}
