#include "point_light_pass.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "function/render/vulkan_rhi/vulkan_rhi_converter.h"
#include "render_pass_common.h"
#include "function/render/shader.h"
#include "function/render/render_mesh.h"
#include "function/render/render_scene.h"
#include "platform/file/file_manager.h"

class PointLightShadowVS : public Shader
{
public:
    DECLARE_SHADER(PointLightShadowVS);
};
class PointLightShadowGS : public Shader
{
public:
    DECLARE_SHADER(PointLightShadowGS);
};
class PointLightShadowPS : public Shader
{
public:
    DECLARE_SHADER(PointLightShadowPS);
};

IMPLEMENT_SHADER_TYPE(PointLightShadowVS, "point_light_shadow.hlsl", "vert", SF_Vertex);
IMPLEMENT_SHADER_TYPE(PointLightShadowGS, "point_light_shadow.hlsl", "geom", SF_Geometry);
IMPLEMENT_SHADER_TYPE(PointLightShadowPS, "point_light_shadow.hlsl", "frag", SF_Pixel);

void PointLightRenderPass::Initialize(const RenderPassInitInfo* init_info)
{
    SetupDescriptorSetLayout();
    SetupAttachments();
    SetupRenderPass();
    SetupFramebuffer();
}

void PointLightRenderPass::PostInitialize()
{
    SetupPipelines();
    SetupDescriptorSet();
}

void PointLightRenderPass::PrepareData(RenderPassPrepareInfo* prepare_info)
{
    PerframeStorageBufferObject = prepare_info->Scene->PointLightShadowPerframeStorageBufferObject;
    VisiableNodes               = &prepare_info->Scene->VisiableNodes;
}

void PointLightRenderPass::Draw()
{
    struct MeshNode
    {
        const Matrix4x4* ModelMatrix {nullptr};
        const Matrix4x4* JointMatrices {nullptr};
        uint32_t         JointCount {0};
    };

    std::map<PBRMaterial*, std::map<RenderMesh*, std::vector<MeshNode>>> mesh_drawcall_batch;

    // reorganize mesh
    for (RenderMeshNode& node : *VisiableNodes->PointLightsVisibleMeshNodes)
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


    RHI->PushEvent(RHI->GetCommandBuffer(), "Point Light Shadow", {1.f, 1.f, 1.f, 1.f});

    RHI->BindPipeline(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[0].PipelineRHI.get());

    auto draw_light_face = [this, &mesh_drawcall_batch](uint32_t light_index, uint32_t face) {
        // Mesh
        RHI->PushEvent(RHI->GetCommandBuffer(), "Mesh", {1.f, 1.f, 1.f, 1.f});

        // point light shadow begin pass
        std::array<RHIClearValue, 2> clear_values {};
        clear_values[0].color.setFloat32({1.f});
        clear_values[1].depthStencil = RHIClearDepthStencilValue {1.f, 0};

        RHIRenderPassBeginInfo render_pass_begin_info {
            .renderPass      = *(VulkanRenderPass*)Framebuffer.RenderPass.get(),
            .framebuffer     = *(VulkanFramebuffer*)ShadowCubeMapFaceFramebuffers[face + 6 * light_index].get(),
            .renderArea      = {0, 0, PointLightShadowMapSize, PointLightShadowMapSize},
            .clearValueCount = clear_values.size(),
            .pClearValues    = clear_values.data(),
        };

        RHI->BeginRenderPass(RHI->GetCommandBuffer(), &render_pass_begin_info, RHISubpassContents::eInline);

        if (light_index >= PerframeStorageBufferObject.PointLightNum)
        {
            RHI->EndRenderPass(RHI->GetCommandBuffer());
            RHI->PopEvent(RHI->GetCommandBuffer());
            return;
        }

        uint32_t                                         perframe_dynamic_offset;
        MeshPointLightShadowPerframeStorageBufferObject* perframe_storage_buffer_object =
            PassCommon->GlobalRenderResource._StorageBuffers.AllocateFromRingbuffer<MeshPointLightShadowPerframeStorageBufferObject>(RHI->GetCurrentFrameIndex(), perframe_dynamic_offset);
        *perframe_storage_buffer_object = PerframeStorageBufferObject;
        perframe_storage_buffer_object->CurrentFace  = face;
        perframe_storage_buffer_object->CurrentIndex = light_index;

        for (const auto& [material, mesh_instanced] : mesh_drawcall_batch)
        {
            // TODO: render from near to far

            for (const auto& [mesh, mesh_nodes] : mesh_instanced)
            {
                uint32_t total_instance_count = mesh_nodes.size();
                if (total_instance_count)
                {
                    // bind per mesh
                    const RHIDescriptorSet* vertex_blending_descriptor_sets[] {mesh->VertexBlendingDescriptorSet.get()};
                    RHI->BindDescriptorSets(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[0].LayoutRHI.get(), 1, vertex_blending_descriptor_sets, {});

                    const RHIBuffer* vertex_buffers[] {mesh->VertexPositionBuffer.VertexBufferRHI.get()};
                    RHI->BindVertexBuffers(RHI->GetCommandBuffer(), 0, vertex_buffers, std::array {0ull});
                    RHI->BindIndexBuffer(RHI->GetCommandBuffer(), mesh->IndexBuffer.IndexBufferRHI.get(), 0, RHIIndexType::eUint16);

                    uint32_t drawcall_max_instance_count = GMeshPerDrawcallMaxInstanceCount;
                    uint32_t drawcall_count              = RoundUp(total_instance_count, drawcall_max_instance_count) / drawcall_max_instance_count;

                    for (uint32_t drawcall_index = 0; drawcall_index < drawcall_count; drawcall_index++)
                    {
                        uint32_t current_instance_count = std::min(total_instance_count - drawcall_max_instance_count * drawcall_index, drawcall_max_instance_count);

                        // perdrawcall storage buffer
                        uint32_t                                                  perdrawcall_dynamic_offset;
                        MeshDirectionalLightShadowPerdrawcallStorageBufferObject* perdrawcall_storage_buffer_object =
                            PassCommon->GlobalRenderResource._StorageBuffers.AllocateFromRingbuffer<MeshDirectionalLightShadowPerdrawcallStorageBufferObject>(RHI->GetCurrentFrameIndex(),
                                                                                                                                                              perdrawcall_dynamic_offset);

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
                            MeshDirectionalLightShadowPerdrawcallVertexBlendingStorageBufferObject* per_drawcall_vertex_blending_storage_buffer_object =
                                PassCommon->GlobalRenderResource._StorageBuffers.AllocateFromRingbuffer<MeshDirectionalLightShadowPerdrawcallVertexBlendingStorageBufferObject>(
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
                        const RHIDescriptorSet* descriptor_sets[] {DescriptorInfos[0].DescriptorSetRHI.get()};
                        RHI->BindDescriptorSets(RHI->GetCommandBuffer(), RHIPipelineBindPoint::eGraphics, RenderPipelines[0].LayoutRHI.get(), 0, descriptor_sets, dynamic_offsets);

                        RHI->DrawIndexed(RHI->GetCommandBuffer(), mesh->IndexCount, current_instance_count, 0, 0, 0);
                    }
                }
            }
        }

        RHI->EndRenderPass(RHI->GetCommandBuffer());

        RHI->PopEvent(RHI->GetCommandBuffer());

    };

    for (uint32_t light_index = 0; light_index < GMaxPointLightCount; light_index++)
    {
        for (uint32_t face = 0; face < 6; face++)
        {
            draw_light_face(light_index, face);
        }
    }

    RHI->PopEvent(RHI->GetCommandBuffer());
}

RHIImageViewRef PointLightRenderPass::GetPointLightShadowMap() const
{
    return Framebuffer.Attachments[0].ImageViewRHI;
}

void PointLightRenderPass::SetupAttachments()
{
    // color and depth
    Framebuffer.Attachments.resize(2);

    Texture& color_texture = Framebuffer.Attachments[0];
    color_texture.Format   = RHIFormat::eR32Sfloat;
    RHI->CreateImage(PointLightShadowMapSize,
                     PointLightShadowMapSize,
                     color_texture.Format,
                     RHIImageTiling::eOptimal,
                     RHIImageUsageFlagBits::eColorAttachment | RHIImageUsageFlagBits::eSampled,
                     RHIMemoryPropertyFlagBits::eDeviceLocal,
                     color_texture.ImageRHI,
                     color_texture.DeviceMemoryRHI,
                     RHIImageCreateFlagBits::eCubeCompatible,
                     6 * GMaxPointLightCount);
    RHI->CreateImageView(color_texture.ImageRHI.get(), color_texture.Format, RHIImageAspectFlagBits::eColor, RHIImageViewType::eCubeArray, 6 * GMaxPointLightCount, 1, color_texture.ImageViewRHI);

    Texture& depth_texture = Framebuffer.Attachments[1];
    depth_texture.Format   = RHI->GetDepthImageInfo().DepthImageFormat;
    RHI->CreateImage(PointLightShadowMapSize,
                     PointLightShadowMapSize,
                     depth_texture.Format,
                     RHIImageTiling::eOptimal,
                     RHIImageUsageFlagBits::eDepthStencilAttachment | RHIImageUsageFlagBits::eTransientAttachment,
                     RHIMemoryPropertyFlagBits::eDeviceLocal,
                     depth_texture.ImageRHI,
                     depth_texture.DeviceMemoryRHI);
    RHI->CreateImageView(depth_texture.ImageRHI.get(), depth_texture.Format, RHIImageAspectFlagBits::eDepth, RHIImageViewType::e2D, 1, 1, depth_texture.ImageViewRHI);
}

void PointLightRenderPass::SetupFramebuffer()
{
    VulkanRHI* vk_rhi = (VulkanRHI*)RHI.get();
    for (uint32_t light_index = 0; light_index < GMaxPointLightCount; light_index++)
    {
        for (uint32_t face = 0; face < 6; face++)
        {
            RHIImageSubresourceRange subresource_range {
                .aspectMask     = RHIImageAspectFlagBits::eColor,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = face + light_index * 6,
                .layerCount     = 1,
            };

            RHIImageViewCreateInfo view_info {
                .image            = VulkanRHIConverter::Convert(*Framebuffer.Attachments[0].ImageRHI),
                .viewType         = RHIImageViewType::e2D,
                .format           = Framebuffer.Attachments[0].Format,
                .subresourceRange = subresource_range,
            };

            auto vk_image_view = vk_rhi->Device->createImageView(view_info);

            std::array<vk::ImageView, 2> vk_attachments {vk_image_view, *(VulkanImageView*)Framebuffer.Attachments[1].ImageViewRHI.get()};
            RHIFramebufferCreateInfo     create_info {
                    .renderPass      = *(VulkanRenderPass*)Framebuffer.RenderPass.get(),
                    .attachmentCount = vk_attachments.size(),
                    .pAttachments    = vk_attachments.data(),
                    .width           = PointLightShadowMapSize,
                    .height          = PointLightShadowMapSize,
                    .layers          = 1,
            };

            ShadowCubeMapFaceImageViews[face + light_index * 6]   = RHIImageViewRef(new VulkanImageView(vk_image_view));
            ShadowCubeMapFaceFramebuffers[face + light_index * 6] = RHIFramebufferRef(RHI->CreateFramebuffer(&create_info));
        }
    }
}

void PointLightRenderPass::SetupDescriptorSetLayout()
{
    DescriptorInfos.resize(1);

    std::array<RHIDescriptorSetLayoutBinding, 3> layout_bindings;

    RHIDescriptorSetLayoutBinding& perframe_storage_buffer_binding = layout_bindings[0];
    perframe_storage_buffer_binding.binding                        = 0;
    perframe_storage_buffer_binding.descriptorType                 = RHIDescriptorType::eStorageBufferDynamic;
    perframe_storage_buffer_binding.descriptorCount                = 1;
    perframe_storage_buffer_binding.stageFlags                     = RHIShaderStageFlagBits::eVertex | RHIShaderStageFlagBits::eFragment;

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

void PointLightRenderPass::SetupDescriptorSet()
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
        .range  = sizeof(MeshPointLightShadowPerframeStorageBufferObject),
    };
    ASSERT(perframe_storage_buffer_info.range < PassCommon->GlobalRenderResource._StorageBuffers.MaxStorageBufferRange);

    RHIDescriptorBufferInfo perdrawcall_storage_buffer_info {
        .buffer = *(VulkanBuffer*)PassCommon->GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffer.BufferRHI.get(),
        .offset = 0,
        .range  = sizeof(MeshPointLightShadowPerdrawcallStorageBufferObject),
    };
    ASSERT(perdrawcall_storage_buffer_info.range < PassCommon->GlobalRenderResource._StorageBuffers.MaxStorageBufferRange);

    RHIDescriptorBufferInfo perdrawcall_vertex_blending_storage_buffer_info {
        .buffer = *(VulkanBuffer*)PassCommon->GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffer.BufferRHI.get(),
        .offset = 0,
        .range  = sizeof(MeshPointLightShadowPerdrawcallVertexBlendingStorageBufferObject),
    };
    ASSERT(perdrawcall_vertex_blending_storage_buffer_info.range < PassCommon->GlobalRenderResource._StorageBuffers.MaxStorageBufferRange);

    RHIDescriptorSet* descriptor_set = DescriptorInfos[0].DescriptorSetRHI.get();

    std::array<RHIWriteDescriptorSet, 3> descriptor_writes {};

    RHIWriteDescriptorSet& perframe_storage_buffer_write = descriptor_writes[0];
    perframe_storage_buffer_write.dstSet                 = VulkanRHIConverter::Convert(*descriptor_set);
    perframe_storage_buffer_write.dstBinding             = 0;
    perframe_storage_buffer_write.dstArrayElement        = 0;
    perframe_storage_buffer_write.descriptorType         = RHIDescriptorType::eStorageBufferDynamic;
    perframe_storage_buffer_write.descriptorCount        = 1;
    perframe_storage_buffer_write.pBufferInfo            = &perframe_storage_buffer_info;

    RHIWriteDescriptorSet& perdrawcall_storage_buffer_write = descriptor_writes[1];
    perdrawcall_storage_buffer_write.dstSet                 = VulkanRHIConverter::Convert(*descriptor_set);
    perdrawcall_storage_buffer_write.dstBinding             = 1;
    perdrawcall_storage_buffer_write.dstArrayElement        = 0;
    perdrawcall_storage_buffer_write.descriptorType         = RHIDescriptorType::eStorageBufferDynamic;
    perdrawcall_storage_buffer_write.descriptorCount        = 1;
    perdrawcall_storage_buffer_write.pBufferInfo            = &perdrawcall_storage_buffer_info;

    RHIWriteDescriptorSet& perdrawcall_vertex_blending_storage_buffer_write = descriptor_writes[2];
    perdrawcall_vertex_blending_storage_buffer_write.dstSet                 = VulkanRHIConverter::Convert(*descriptor_set);
    perdrawcall_vertex_blending_storage_buffer_write.dstBinding             = 2;
    perdrawcall_vertex_blending_storage_buffer_write.dstArrayElement        = 0;
    perdrawcall_vertex_blending_storage_buffer_write.descriptorType         = RHIDescriptorType::eStorageBufferDynamic;
    perdrawcall_vertex_blending_storage_buffer_write.descriptorCount        = 1;
    perdrawcall_vertex_blending_storage_buffer_write.pBufferInfo            = &perdrawcall_vertex_blending_storage_buffer_info;

    RHI->UpdateDescriptorSets(descriptor_writes, {});
}

void PointLightRenderPass::SetupRenderPass()
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

    RHIAttachmentDescription& depth_attachment = attachments[1];
    depth_attachment.format                    = Framebuffer.Attachments[1].Format;
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
    RHISubpassDescription&               shadow_pass = subpasses[0];
    shadow_pass.pipelineBindPoint                    = RHIPipelineBindPoint::eGraphics;
    shadow_pass.colorAttachmentCount                 = 1;
    shadow_pass.pColorAttachments                    = &color_attachment_reference;
    shadow_pass.pDepthStencilAttachment              = &depth_attachment_reference;

    std::array<RHISubpassDependency, 1> dependencies {};
    RHISubpassDependency&               lighting_pass_denpendency = dependencies[0];
    lighting_pass_denpendency.srcSubpass                          = 0;
    lighting_pass_denpendency.dstSubpass                          = RHI_SUBPASS_EXTERNAL;
    lighting_pass_denpendency.srcStageMask                        = RHIPipelineStageFlagBits::eColorAttachmentOutput;
    lighting_pass_denpendency.dstStageMask                        = RHIPipelineStageFlagBits::eBottomOfPipe;
    lighting_pass_denpendency.srcAccessMask                       = RHIAccessFlagBits::eColorAttachmentWrite;

    RHIRenderPassCreateInfo render_pass_create_info {};
    render_pass_create_info.attachmentCount = attachments.size();
    render_pass_create_info.pAttachments    = attachments.data();
    render_pass_create_info.subpassCount    = subpasses.size();
    render_pass_create_info.pSubpasses      = subpasses.data();
    render_pass_create_info.dependencyCount = dependencies.size();
    render_pass_create_info.pDependencies   = dependencies.data();

    Framebuffer.RenderPass = RHIRenderPassRef(RHI->CreateRenderPass(&render_pass_create_info));
}

void PointLightRenderPass::SetupPipelines()
{
    RenderPipelines.resize(1);

    std::array<RHIDescriptorSetLayout*, 2> descriptor_layouts {DescriptorInfos[0].LayoutRHI.get(), PassCommon->PerMeshLayout.get()};
    std::array<vk::DescriptorSetLayout, 2> vk_descriptor_layouts {*(VulkanDescriptorSetLayout*)descriptor_layouts[0], *(VulkanDescriptorSetLayout*)descriptor_layouts[1]};
    RHIPipelineLayoutCreateInfo            pipeline_layout_create_info {
                   .setLayoutCount = descriptor_layouts.size(),
                   .pSetLayouts    = vk_descriptor_layouts.data(),
    };

    RenderPipelines[0].LayoutRHI = RHIPipelineLayoutRef(RHI->CreatePipelineLayout(&pipeline_layout_create_info));

    RHIShader* vert_shader_module = RHI->CreateShaderModule(FileManager::Read(PointLightShadowVS::StaticType.GetCachedFilePath()));
    RHIShader* frag_shader_module = RHI->CreateShaderModule(FileManager::Read(PointLightShadowPS::StaticType.GetCachedFilePath()));

    RHIPipelineShaderStageCreateInfo vert_pipeline_shader_stage_create_info {
        .stage  = RHIShaderStageFlagBits::eVertex,
        .module = *(VulkanShader*)vert_shader_module,
        .pName  = PointLightShadowVS::StaticType.EntryPoint.c_str(),
    };
    RHIPipelineShaderStageCreateInfo frag_pipeline_shader_stage_create_info {
        .stage  = RHIShaderStageFlagBits::eFragment,
        .module = *(VulkanShader*)frag_shader_module,
        .pName  = PointLightShadowPS::StaticType.EntryPoint.c_str(),
    };

    std::array<RHIPipelineShaderStageCreateInfo, 2> shader_stages {vert_pipeline_shader_stage_create_info, frag_pipeline_shader_stage_create_info};

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
        .width    = PointLightShadowMapSize,
        .height   = PointLightShadowMapSize,
        .minDepth = 0.0,
        .maxDepth = 1.0,
    };
    RHIRect2D scissor {.offset = {0, 0}, .extent = {PointLightShadowMapSize, PointLightShadowMapSize}};

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
        .frontFace               = RHIFrontFace::eClockwise,
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
