#include "tone_mapping_pass.h"
#include "function/render/vulkan_rhi/vulkan_rhi_converter.h"
#include "platform/file/file_manager.h"
#include "function/render/shader.h"
#include "function/render/shader_common.h"

class ToneMappingPS : public Shader
{
public:
    DECLARE_SHADER(ToneMappingPS);
};

IMPLEMENT_SHADER_TYPE(ToneMappingPS, "tone_mapping.hlsl", "frag", SF_Pixel);

void ToneMappingRenderPass::Initialize(const RenderPassInitInfo* init_info)
{
    const ToneMappingPassInitInfo* mesh_pass_init_info = static_cast<const ToneMappingPassInitInfo*>(init_info);

    Framebuffer.RenderPass = mesh_pass_init_info->RenderPass;

    SetupDescriptorSetLayout();
    SetupPipelines();
    SetupDescriptorSet();
    FramebufferRecreateInfo tmp_info;
    tmp_info.PostProcessAttachment = mesh_pass_init_info->InputAttachment.get();
    UpdateAfterFramebufferRecreate(&tmp_info);
}

void ToneMappingRenderPass::PrepareData(RenderPassPrepareInfo* prepare_info) {}

void ToneMappingRenderPass::UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info)
{
    RHIDescriptorImageInfo post_process_input_attachment_info {};
    post_process_input_attachment_info.sampler     = VulkanRHIConverter::Convert(*RHI->GetOrCreateDefaultSampler(RHIDefaultSamplerType::Nearest));
    post_process_input_attachment_info.imageView   = VulkanRHIConverter::Convert(*recreate_info->PostProcessAttachment);
    post_process_input_attachment_info.imageLayout = RHIImageLayout::eShaderReadOnlyOptimal;

    std::array<RHIWriteDescriptorSet, 1> descriptor_writes {};

    RHIWriteDescriptorSet& post_process_input_attachment_write = descriptor_writes[0];
    post_process_input_attachment_write.dstSet                 = VulkanRHIConverter::Convert(*DescriptorInfos[0].DescriptorSetRHI);
    post_process_input_attachment_write.dstBinding             = 0;
    post_process_input_attachment_write.dstArrayElement        = 0;
    post_process_input_attachment_write.descriptorType         = RHIDescriptorType::eInputAttachment;
    post_process_input_attachment_write.descriptorCount        = 1;
    post_process_input_attachment_write.pImageInfo             = &post_process_input_attachment_info;

    RHI->UpdateDescriptorSets(descriptor_writes, {});
}

void ToneMappingRenderPass::Draw()
{
    auto swapchain_info = RHI->GetSwapchainInfo();

    RHI->PushEvent(RHI->GetCommandBuffer(), "Tone Mapping", {1.f, 1.f, 1.f, 1.f});

    RHI->BindPipeline(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[0].PipelineRHI.get());
    RHI->SetViewport(RHI->GetCommandBuffer(), 0, std::array {*swapchain_info.Viewport});
    RHI->SetScissor(RHI->GetCommandBuffer(), 0, std::array {*swapchain_info.Scissor});
    const RHIDescriptorSet* tone_mapping_descriptor_sets[] {DescriptorInfos[0].DescriptorSetRHI.get()};
    RHI->BindDescriptorSets(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[0].LayoutRHI.get(), 0, tone_mapping_descriptor_sets, {});

    RHI->Draw(RHI->GetCommandBuffer(), 3, 1, 0, 0);

    RHI->PopEvent(RHI->GetCommandBuffer());
}

void ToneMappingRenderPass::SetupDescriptorSetLayout()
{
    DescriptorInfos.resize(1);

    std::array<RHIDescriptorSetLayoutBinding, 1> post_process_layout_bindings;

    RHIDescriptorSetLayoutBinding& post_process_binding = post_process_layout_bindings[0];
    post_process_binding.binding                        = 0;
    post_process_binding.descriptorType                 = RHIDescriptorType::eInputAttachment;
    post_process_binding.descriptorCount                = 1;
    post_process_binding.stageFlags                     = RHIShaderStageFlagBits::eFragment;

    
    RHIDescriptorSetLayoutCreateInfo layout_create_info {
        .bindingCount = post_process_layout_bindings.size(),
        .pBindings    = post_process_layout_bindings.data(),
    };

    DescriptorInfos[0].LayoutRHI = RHIDescriptorSetLayoutRef(RHI->CreateDescriptorSetLayout(&layout_create_info));
}

void ToneMappingRenderPass::SetupDescriptorSet()
{
    RHIDescriptorSetAllocateInfo mesh_global_descriptor_set_alloc_info {
        .descriptorPool     = VulkanRHIConverter::Convert(*RHI->GetDescriptorPool()),
        .descriptorSetCount = 1,
        .pSetLayouts        = (vk::DescriptorSetLayout*)DescriptorInfos[0].LayoutRHI.get(),
    };

    DescriptorInfos[0].DescriptorSetRHI = RHIDescriptorSetRef(RHI->AllocateDescriptorSets(&mesh_global_descriptor_set_alloc_info));
}

void ToneMappingRenderPass::SetupPipelines()
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

    RHIShader* vert_shader_module = RHI->CreateShaderModule(FileManager::Read(PostProcessVS::StaticType.GetCachedFilePath()));
    RHIShader* frag_shader_module = RHI->CreateShaderModule(FileManager::Read(ToneMappingPS::StaticType.GetCachedFilePath()));

    RHIPipelineShaderStageCreateInfo vert_pipeline_shader_stage_create_info {
        .stage  = RHIShaderStageFlagBits::eVertex,
        .module = VulkanRHIConverter::Convert(*vert_shader_module),
        .pName  = PostProcessVS::StaticType.EntryPoint.c_str(),
    };
    RHIPipelineShaderStageCreateInfo frag_pipeline_shader_stage_create_info {
        .stage  = RHIShaderStageFlagBits::eFragment,
        .module = VulkanRHIConverter::Convert(*frag_shader_module),
        .pName  = ToneMappingPS::StaticType.EntryPoint.c_str(),
    };

    std::array<RHIPipelineShaderStageCreateInfo, 2> shader_stages {vert_pipeline_shader_stage_create_info, frag_pipeline_shader_stage_create_info};
    
    RHIPipelineVertexInputStateCreateInfo vertex_input_state_create_info {
        .vertexBindingDescriptionCount   = 0,
        .pVertexBindingDescriptions      = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions    = nullptr,
    };

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
        .cullMode                = RHICullModeFlagBits::eNone,
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
        .subpass             = _subpass_tone_mapping,
    };

    RenderPipelines[0].PipelineRHI = RHIPipelineRef(RHI->CreateGraphicsPipeline(RHI_NULL_HANDLE, &pipeline_info));

    RHI->DestroyShaderModule(vert_shader_module);
    RHI->DestroyShaderModule(frag_shader_module);
}
