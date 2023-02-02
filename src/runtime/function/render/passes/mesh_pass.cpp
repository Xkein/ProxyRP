#include "mesh_pass.h"
#include "function/render/shader.h"
#include "function/render/passes/render_pass_common.h"
#include "function/render/render_scene.h"
#include "function/render/vulkan_rhi/vulkan_rhi_converter.h"
#include "platform/file/file_manager.h"
#include <function/render/render_mesh.h>

class MeshVS : public Shader
{
public:
    DECLARE_SHADER(MeshVS);
};
class MeshPS : public Shader
{
public:
    DECLARE_SHADER(MeshPS);
};
class MeshGBufferPS : public Shader
{
public:
    DECLARE_SHADER(MeshGBufferPS);
};
class DeferredLightingVS : public Shader
{
public:
    DECLARE_SHADER(MeshGBufferVS);
};
class DeferredLightingPS : public Shader
{
public:
    DECLARE_SHADER(MeshGBufferPS);
};

IMPLEMENT_SHADER_TYPE(MeshVS, "mesh.hlsl", "vert", SF_Vertex);
IMPLEMENT_SHADER_TYPE(MeshPS, "mesh.hlsl", "frag", SF_Pixel);
IMPLEMENT_SHADER_TYPE(MeshGBufferPS, "gbuffer.hlsl", "frag", SF_Pixel);
IMPLEMENT_SHADER_TYPE(DeferredLightingVS, "deferred_lighting.hlsl", "vert", SF_Vertex);
IMPLEMENT_SHADER_TYPE(DeferredLightingPS, "deferred_lighting.hlsl", "frag", SF_Pixel);


void MeshPass::Initialize(const RenderPassInitInfo* init_info)
{
    const MeshPassInitInfo* mesh_pass_init_info = static_cast<const MeshPassInitInfo*>(init_info);

    DirectionalLightShadowMap = mesh_pass_init_info->DirectionalLightShadowMap;

    SetupDescriptorSetLayout();
    SetupAttachments();
    SetupRenderPass();
    SetupFramebuffer();
    SetupPipelines();
    SetupDescriptorSet();
    SetupFramebufferDescriptorSet();
}

void MeshPass::PostInitialize()
{
}

void MeshPass::PrepareData(RenderPassPrepareInfo* prepare_info)
{
    PerframeStorageBufferObject = prepare_info->Scene->PerframeStorageBufferObject;
    VisiableNodes               = &prepare_info->Scene->VisiableNodes;
}

void MeshPass::UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info)
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

void MeshPass::Draw()
{
    auto swapchain_info = RHI->GetSwapchainInfo();

    std::array<RHIClearValue, _pass_attachment_count> clear_values {};
    clear_values[_pass_attachment_depth].depthStencil = RHIClearDepthStencilValue {1.f, 0};
    clear_values[_pass_attachment_swap_chain_image].color.setFloat32({1.f});

    RHIRenderPassBeginInfo render_pass_begin_info {
        .renderPass      = VulkanRHIConverter::Convert(*Framebuffer.RenderPass),
        .framebuffer     = VulkanRHIConverter::Convert(*SwapchainFramebuffers[RHI->GetCurrentFrameIndex()]),
        .renderArea      = {0, 0, swapchain_info.Extent},
        .clearValueCount = clear_values.size(),
        .pClearValues    = clear_values.data(),
    };

    RHI->BeginRenderPass(RHI->GetCommandBuffer(), &render_pass_begin_info, RHISubpassContents::eInline);

    RHI->PushEvent(RHI->GetCommandBuffer(), "Forward Lighting", {1.f, 1.f, 1.f, 1.f});

    DrawMeshLighting();

    RHI->PopEvent(RHI->GetCommandBuffer());

    RHI->EndRenderPass(RHI->GetCommandBuffer());
}


void MeshPass::SetupAttachments()
{
    Framebuffer.Attachments.resize(_pass_attachment_count);

    Framebuffer.Attachments[_pass_attachment_depth].Format = RHI->GetDepthImageInfo().DepthImageFormat;

    Framebuffer.Attachments[_pass_attachment_swap_chain_image].Format = RHI->GetSwapchainInfo().ImageFormat;
}

void MeshPass::SetupFramebuffer()
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

void MeshPass::SetupFramebufferDescriptorSet() {}

void MeshPass::SetupDescriptorSetLayout()
{
    DescriptorInfos.resize(_layout_count);

    DescriptorInfos[_layout_per_mesh].LayoutRHI = PassCommon->PerMeshLayout;

    std::array<RHIDescriptorSetLayoutBinding, 5> mesh_global_layout_bindings;

    RHIDescriptorSetLayoutBinding& perframe_storage_buffer_binding = mesh_global_layout_bindings[0];
    perframe_storage_buffer_binding.binding                        = 0;
    perframe_storage_buffer_binding.descriptorType                 = RHIDescriptorType::eStorageBufferDynamic;
    perframe_storage_buffer_binding.descriptorCount                = 1;
    perframe_storage_buffer_binding.stageFlags                     = RHIShaderStageFlagBits::eVertex | RHIShaderStageFlagBits::eFragment;

    RHIDescriptorSetLayoutBinding& perdrawcall_storage_buffer_binding = mesh_global_layout_bindings[1];
    perdrawcall_storage_buffer_binding.binding                        = 1;
    perdrawcall_storage_buffer_binding.descriptorType                 = RHIDescriptorType::eStorageBufferDynamic;
    perdrawcall_storage_buffer_binding.descriptorCount                = 1;
    perdrawcall_storage_buffer_binding.stageFlags                     = RHIShaderStageFlagBits::eVertex;

    RHIDescriptorSetLayoutBinding& vertex_blending_storage_buffer_binding = mesh_global_layout_bindings[2];
    vertex_blending_storage_buffer_binding.binding                        = 2;
    vertex_blending_storage_buffer_binding.descriptorType                 = RHIDescriptorType::eStorageBufferDynamic;
    vertex_blending_storage_buffer_binding.descriptorCount                = 1;
    vertex_blending_storage_buffer_binding.stageFlags                     = RHIShaderStageFlagBits::eVertex;

    RHIDescriptorSetLayoutBinding& point_light_shadow_binding = mesh_global_layout_bindings[3];
    point_light_shadow_binding.binding                        = 3;
    point_light_shadow_binding.descriptorType                 = RHIDescriptorType::eCombinedImageSampler;
    point_light_shadow_binding.descriptorCount                = 1;
    point_light_shadow_binding.stageFlags                     = RHIShaderStageFlagBits::eFragment;

    RHIDescriptorSetLayoutBinding& directional_light_shadow_binding = mesh_global_layout_bindings[4];
    directional_light_shadow_binding.binding                        = 4;
    directional_light_shadow_binding.descriptorType                 = RHIDescriptorType::eCombinedImageSampler;
    directional_light_shadow_binding.descriptorCount                = 1;
    directional_light_shadow_binding.stageFlags                     = RHIShaderStageFlagBits::eFragment;

    RHIDescriptorSetLayoutCreateInfo layout_create_info {
        .bindingCount = mesh_global_layout_bindings.size(),
        .pBindings    = mesh_global_layout_bindings.data(),
    };

    DescriptorInfos[_layout_mesh_global].LayoutRHI = RHIDescriptorSetLayoutRef(RHI->CreateDescriptorSetLayout(&layout_create_info));


    DescriptorInfos[_layout_mesh_per_material].LayoutRHI = PassCommon->MaterialLayout;
}

void MeshPass::SetupDescriptorSet() 
{
    RHIDescriptorSetAllocateInfo mesh_global_descriptor_set_alloc_info {
        .descriptorPool     = VulkanRHIConverter::Convert(*RHI->GetDescriptorPool()),
        .descriptorSetCount = 1,
        .pSetLayouts        = (vk::DescriptorSetLayout*)DescriptorInfos[_layout_mesh_global].LayoutRHI.get(),
    };

    DescriptorInfos[_layout_mesh_global].DescriptorSetRHI = RHIDescriptorSetRef(RHI->AllocateDescriptorSets(&mesh_global_descriptor_set_alloc_info));

    RHIDescriptorBufferInfo perframe_storage_buffer_info {
        .buffer = VulkanRHIConverter::Convert(*PassCommon->GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffer.BufferRHI),
        .offset = 0,
        .range  = sizeof(MeshPerframeStorageBufferObject),
    };

    RHIDescriptorBufferInfo perdrawcall_storage_buffer_info {
        .buffer = VulkanRHIConverter::Convert(*PassCommon->GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffer.BufferRHI),
        .offset = 0,
        .range  = sizeof(MeshPerdrawcallStorageBufferObject),
    };

    RHIDescriptorBufferInfo perdrawcall_vertex_blending_storage_buffer_info {
        .buffer = VulkanRHIConverter::Convert(*PassCommon->GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffer.BufferRHI),
        .offset = 0,
        .range  = sizeof(MeshPerdrawcallVertexBlendingStorageBufferObject),
    };

    RHIDescriptorImageInfo point_light_shadow_texture_image_info {};
    point_light_shadow_texture_image_info.sampler     = VulkanRHIConverter::Convert(*RHI->GetOrCreateDefaultSampler(RHIDefaultSamplerType::Nearest));
    point_light_shadow_texture_image_info.imageView   = VulkanRHIConverter::Convert(*DirectionalLightShadowMap);
    point_light_shadow_texture_image_info.imageLayout = RHIImageLayout::eShaderReadOnlyOptimal;

    RHIDescriptorImageInfo directional_light_shadow_texture_image_info {};
    directional_light_shadow_texture_image_info.sampler     = VulkanRHIConverter::Convert(*RHI->GetOrCreateDefaultSampler(RHIDefaultSamplerType::Nearest));
    directional_light_shadow_texture_image_info.imageView   = VulkanRHIConverter::Convert(*DirectionalLightShadowMap);
    directional_light_shadow_texture_image_info.imageLayout = RHIImageLayout::eShaderReadOnlyOptimal;

    RHIDescriptorSet* mesh_globaldescriptor_set = DescriptorInfos[_layout_mesh_global].DescriptorSetRHI.get();

    std::array<RHIWriteDescriptorSet, 5> descriptor_writes {};

    RHIWriteDescriptorSet& perframe_storage_buffer_write = descriptor_writes[0];
    perframe_storage_buffer_write.dstSet                 = VulkanRHIConverter::Convert(*mesh_globaldescriptor_set);
    perframe_storage_buffer_write.dstBinding             = 0;
    perframe_storage_buffer_write.dstArrayElement        = 0;
    perframe_storage_buffer_write.descriptorType         = RHIDescriptorType::eStorageBufferDynamic;
    perframe_storage_buffer_write.descriptorCount        = 1;
    perframe_storage_buffer_write.pBufferInfo            = &perframe_storage_buffer_info;

    RHIWriteDescriptorSet& perdrawcall_storage_buffer_write = descriptor_writes[1];
    perdrawcall_storage_buffer_write.dstSet                 = VulkanRHIConverter::Convert(*mesh_globaldescriptor_set);
    perdrawcall_storage_buffer_write.dstBinding             = 1;
    perdrawcall_storage_buffer_write.dstArrayElement        = 0;
    perdrawcall_storage_buffer_write.descriptorType         = RHIDescriptorType::eStorageBufferDynamic;
    perdrawcall_storage_buffer_write.descriptorCount        = 1;
    perdrawcall_storage_buffer_write.pBufferInfo            = &perdrawcall_storage_buffer_info;

    RHIWriteDescriptorSet& perdrawcall_vertex_blending_storage_buffer_write = descriptor_writes[2];
    perdrawcall_vertex_blending_storage_buffer_write.dstSet                 = VulkanRHIConverter::Convert(*mesh_globaldescriptor_set);
    perdrawcall_vertex_blending_storage_buffer_write.dstBinding             = 2;
    perdrawcall_vertex_blending_storage_buffer_write.dstArrayElement        = 0;
    perdrawcall_vertex_blending_storage_buffer_write.descriptorType         = RHIDescriptorType::eStorageBufferDynamic;
    perdrawcall_vertex_blending_storage_buffer_write.descriptorCount        = 1;
    perdrawcall_vertex_blending_storage_buffer_write.pBufferInfo            = &perdrawcall_vertex_blending_storage_buffer_info;

    RHIWriteDescriptorSet& point_light_shadow_texture_image_write = descriptor_writes[3];
    point_light_shadow_texture_image_write.dstSet                 = VulkanRHIConverter::Convert(*mesh_globaldescriptor_set);
    point_light_shadow_texture_image_write.dstBinding             = 3;
    point_light_shadow_texture_image_write.dstArrayElement        = 0;
    point_light_shadow_texture_image_write.descriptorType         = RHIDescriptorType::eCombinedImageSampler;
    point_light_shadow_texture_image_write.descriptorCount        = 1;
    point_light_shadow_texture_image_write.pImageInfo             = &point_light_shadow_texture_image_info;

    RHIWriteDescriptorSet& directional_light_shadow_texture_image_write = descriptor_writes[4];
    directional_light_shadow_texture_image_write.dstSet                 = VulkanRHIConverter::Convert(*mesh_globaldescriptor_set);
    directional_light_shadow_texture_image_write.dstBinding             = 4;
    directional_light_shadow_texture_image_write.dstArrayElement        = 0;
    directional_light_shadow_texture_image_write.descriptorType         = RHIDescriptorType::eCombinedImageSampler;
    directional_light_shadow_texture_image_write.descriptorCount        = 1;
    directional_light_shadow_texture_image_write.pImageInfo             = &directional_light_shadow_texture_image_info;

    RHI->UpdateDescriptorSets(descriptor_writes, {});
}

void MeshPass::SetupRenderPass()
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
    color_attachment.finalLayout               = RHIImageLayout::eShaderReadOnlyOptimal;

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

    RHISubpassDescription& forward_lighting_pass   = subpasses[_subpass_forward_lighting];
    forward_lighting_pass.pipelineBindPoint        = RHIPipelineBindPoint::eGraphics;
    forward_lighting_pass.colorAttachmentCount     = 1;
    forward_lighting_pass.pColorAttachments        = &color_attachment_reference;
    forward_lighting_pass.pDepthStencilAttachment  = &depth_attachment_reference;

    std::array<RHISubpassDependency, 1> dependencies {};

    RHISubpassDependency& forward_lighting_pass_denpendency = dependencies[0];
    forward_lighting_pass_denpendency.srcSubpass            = VK_SUBPASS_EXTERNAL;
    forward_lighting_pass_denpendency.dstSubpass            = _subpass_forward_lighting;
    forward_lighting_pass_denpendency.srcStageMask          = RHIPipelineStageFlagBits::eColorAttachmentOutput;
    forward_lighting_pass_denpendency.dstStageMask          = RHIPipelineStageFlagBits::eFragmentShader| RHIPipelineStageFlagBits::eColorAttachmentOutput;
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

void MeshPass::SetupPipelines()
{
    auto swapchain_info = RHI->GetSwapchainInfo();

    RenderPipelines.resize(_pipeline_count);

    std::array<RHIDescriptorSetLayout*, 3> descriptor_layouts {
        DescriptorInfos[_layout_mesh_global].LayoutRHI.get(), DescriptorInfos[_layout_per_mesh].LayoutRHI.get(), DescriptorInfos[_layout_mesh_per_material].LayoutRHI.get()};
    auto                        vk_descriptor_layouts = VulkanRHIConverter::Convert(descriptor_layouts);
    RHIPipelineLayoutCreateInfo            pipeline_layout_create_info {
                   .setLayoutCount = descriptor_layouts.size(),
                   .pSetLayouts    = vk_descriptor_layouts.data(),
    };

    RenderPipelines[_pipeline_mesh_lighting].LayoutRHI = RHIPipelineLayoutRef(RHI->CreatePipelineLayout(&pipeline_layout_create_info));

    RHIShader* vert_shader_module = RHI->CreateShaderModule(FileManager::Read(MeshVS::StaticType.GetCachedFilePath()));
    RHIShader* frag_shader_module = RHI->CreateShaderModule(FileManager::Read(MeshPS::StaticType.GetCachedFilePath()));

    RHIPipelineShaderStageCreateInfo vert_pipeline_shader_stage_create_info {
        .stage  = RHIShaderStageFlagBits::eVertex,
        .module = *(VulkanShader*)vert_shader_module,
        .pName  = MeshVS::StaticType.EntryPoint.c_str(),
    };
    RHIPipelineShaderStageCreateInfo frag_pipeline_shader_stage_create_info {
        .stage  = RHIShaderStageFlagBits::eFragment,
        .module = *(VulkanShader*)frag_shader_module,
        .pName  = MeshPS::StaticType.EntryPoint.c_str(),
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
        .layout              = *(VulkanPipelineLayout*)RenderPipelines[_pipeline_mesh_lighting].LayoutRHI.get(),
        .renderPass          = *(VulkanRenderPass*)Framebuffer.RenderPass.get(),
        .subpass             = _subpass_forward_lighting,
    };

    RenderPipelines[_pipeline_mesh_lighting].PipelineRHI = RHIPipelineRef(RHI->CreateGraphicsPipeline(RHI_NULL_HANDLE, &pipeline_info));

    RHI->DestroyShaderModule(vert_shader_module);
    RHI->DestroyShaderModule(frag_shader_module);

}

void MeshPass::DrawMeshLighting()
{
    struct MeshNode
    {
        const Matrix4x4* ModelMatrix {nullptr};
        const Matrix4x4* JointMatrices {nullptr};
        uint32_t         JointCount {0};
    };

    std::map<PBRMaterial*, std::map<RenderMesh*, std::vector<MeshNode>>> mesh_drawcall_batch;

    // reorganize mesh
    for (RenderMeshNode& node : *VisiableNodes->MainCameraVisibleMeshNodes)
    {
        auto& mesh_instanced = mesh_drawcall_batch[node.RefMaterial];
        auto& mesh_nodes     = mesh_instanced[node.RefMesh];

        MeshNode temp;
        temp.ModelMatrix = node.ModelMatrix;
        if (node.EnableVertexBlending)
        {
            temp.JointMatrices = node.JointMatrices;
            temp.JointCount    = node.JointCount;
        }

        mesh_nodes.push_back(temp);
    }

    auto swapchain_info = RHI->GetSwapchainInfo();


    RHI->PushEvent(RHI->GetCommandBuffer(), "Mesh Lighting", {1.f, 1.f, 1.f, 1.f});

    RHI->BindPipeline(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[_pipeline_mesh_lighting].PipelineRHI.get());

    RHI->SetViewport(RHI->GetCommandBuffer(), 0, std::array {*swapchain_info.Viewport});
    RHI->SetScissor(RHI->GetCommandBuffer(), 0, std::array {*swapchain_info.Scissor});

    
    uint32_t                                               perframe_dynamic_offset;
    MeshPerframeStorageBufferObject* perframe_storage_buffer_object =
        PassCommon->GlobalRenderResource._StorageBuffers.AllocateFromRingbuffer<MeshPerframeStorageBufferObject>(RHI->GetCurrentFrameIndex(), perframe_dynamic_offset);
    *perframe_storage_buffer_object = PerframeStorageBufferObject;
    
    
    for (auto& [material, mesh_instanced] : mesh_drawcall_batch)
    {
        const RHIDescriptorSet* material_descriptor_sets[] {material->DescriptorSet.get()};
        RHI->BindDescriptorSets(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[_pipeline_mesh_lighting].LayoutRHI.get(), 2, material_descriptor_sets, {});

        for (auto& [mesh, mesh_nodes] : mesh_instanced)
        {
            uint32_t total_instance_count = mesh_nodes.size();
            if (total_instance_count)
            {
                // bind per mesh
                const RHIDescriptorSet* vertex_blending_descriptor_sets[] {mesh->VertexBlendingDescriptorSet.get()};
                RHI->BindDescriptorSets(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[_pipeline_mesh_lighting].LayoutRHI.get(), 1, vertex_blending_descriptor_sets, {});

                const RHIBuffer* vertex_buffers[] {
                    mesh->VertexPositionBuffer.VertexBufferRHI.get(), mesh->VertexVaryingEnableBlendingBuffer.VertexBufferRHI.get(), mesh->VertexVaryingBuffer.VertexBufferRHI.get()};
                RHI->BindVertexBuffers(RHI->GetCommandBuffer(), 0, vertex_buffers, std::array<RHIDeviceSize, 3> {});
                RHI->BindIndexBuffer(RHI->GetCommandBuffer(), mesh->IndexBuffer.IndexBufferRHI.get(), 0, RHIIndexType::eUint16);

                uint32_t drawcall_max_instance_count = GMeshPerDrawcallMaxInstanceCount;
                uint32_t drawcall_count              = RoundUp(total_instance_count, drawcall_max_instance_count) / drawcall_max_instance_count;

                for (uint32_t drawcall_index = 0; drawcall_index < drawcall_count; drawcall_index++)
                {
                    uint32_t current_instance_count = std::min(total_instance_count - drawcall_max_instance_count * drawcall_index, drawcall_max_instance_count);

                    // perdrawcall storage buffer
                    uint32_t                            perdrawcall_dynamic_offset;
                    MeshPerdrawcallStorageBufferObject* perdrawcall_storage_buffer_object =
                        PassCommon->GlobalRenderResource._StorageBuffers.AllocateFromRingbuffer<MeshPerdrawcallStorageBufferObject>(RHI->GetCurrentFrameIndex(), perdrawcall_dynamic_offset);

                    for (uint32_t i = 0; i < current_instance_count; i++)
                    {
                        perdrawcall_storage_buffer_object->MeshInstances[i].ModelMatrix          = *mesh_nodes[drawcall_max_instance_count * drawcall_index + i].ModelMatrix;
                        perdrawcall_storage_buffer_object->MeshInstances[i].EnableVertexBlending = mesh_nodes[drawcall_max_instance_count * drawcall_index + i].JointMatrices ? 1.f : -1.f;
                    }

                    // per drawcall vertex blending storage buffer
                    bool all_enable_vertex_blending = true;
                    for (uint32_t i = 0; i < current_instance_count; i++)
                    {
                        if (!mesh_nodes[drawcall_max_instance_count * drawcall_index + i].JointMatrices)
                        {
                            all_enable_vertex_blending = false;
                            break;
                        }
                    }

                    uint32_t per_drawcall_vertex_blending_dynamic_offset = 0;
                    if (all_enable_vertex_blending)
                    {
                        MeshPerdrawcallVertexBlendingStorageBufferObject* per_drawcall_vertex_blending_storage_buffer_object =
                            PassCommon->GlobalRenderResource._StorageBuffers.AllocateFromRingbuffer<MeshPerdrawcallVertexBlendingStorageBufferObject>(
                                RHI->GetCurrentFrameIndex(), per_drawcall_vertex_blending_dynamic_offset);

                        for (uint32_t i = 0; i < current_instance_count; i++)
                        {
                            if (mesh_nodes[drawcall_max_instance_count * drawcall_index + i].JointMatrices)
                            {
                                for (uint32_t joint_index = 0; joint_index < mesh_nodes[drawcall_max_instance_count * drawcall_index + i].JointCount; joint_index++)
                                {
                                    per_drawcall_vertex_blending_storage_buffer_object->JointMatrices[GMeshVertexBlendingMaxJointCount * i + joint_index] =
                                        mesh_nodes[drawcall_max_instance_count * drawcall_index + i].JointMatrices[joint_index];
                                }
                            }
                        }
                    }

                    std::array<uint32_t, 3> dynamic_offsets {perframe_dynamic_offset, perdrawcall_dynamic_offset, per_drawcall_vertex_blending_dynamic_offset};
                    const RHIDescriptorSet* descriptor_sets[] {DescriptorInfos[_layout_mesh_global].DescriptorSetRHI.get()};
                    RHI->BindDescriptorSets(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[_pipeline_mesh_lighting].LayoutRHI.get(), 0, descriptor_sets, dynamic_offsets);

                    RHI->DrawIndexed(RHI->GetCommandBuffer(), mesh->IndexCount, current_instance_count, 0, 0, 0);
                }
            }
        }

    }

    RHI->PopEvent(RHI->GetCommandBuffer());

}

