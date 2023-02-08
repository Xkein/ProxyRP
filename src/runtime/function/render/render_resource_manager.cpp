#include "render_resource_manager.h"
#include "core/log/log_system.h"
#include "resource/asset/asset_manager.h"
#include "resource/config/config_manager.h"
#include "platform/file/file_manager.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "function/render/render_common.h"
#include "function/render/render_mesh.h"
#include "function/render/passes/render_pass_common.h"

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

RenderResourceManager::RenderResourceManager()
{
    DefaultRegistry  = std::make_shared<AssetRegistry>("RenderResourceManager::Default");
    MeshRegistry     = std::make_shared<AssetRegistry>("RenderResourceManager::Mesh");
    TextureRegistry  = std::make_shared<AssetRegistry>("RenderResourceManager::Texture");
    MaterialRegistry = std::make_shared<AssetRegistry>("RenderResourceManager::Material");
}

void RenderResourceManager::Clear() {}

void RenderResourceManager::UploadGlobalRenderResource(const LevelResourceDesc& level_resource_desc)
{

}

void RenderResourceManager::UploadGameObjectRenderResource(const RenderEntity&       render_entity,
                                                           const RenderMeshData&     mesh_data,
                                                           const RenderMaterialData& material_data)
{
    UploadGameObjectRenderResource(render_entity, mesh_data);
    UploadGameObjectRenderResource(render_entity, material_data);
}

void RenderResourceManager::UploadGameObjectRenderResource(const RenderEntity&   render_entity,
                                                           const RenderMeshData& mesh_data)
{
    auto mesh = GetOrCreateRenderMesh(render_entity, mesh_data);
    __PersistentResources.push_back(std::reinterpret_pointer_cast<void*>(mesh));
}

void RenderResourceManager::UploadGameObjectRenderResource(const RenderEntity&       render_entity,
                                                           const RenderMaterialData& material_data)
{
    auto material = GetOrCreatePBRMaterial(render_entity, material_data);
    __PersistentResources.push_back(std::reinterpret_pointer_cast<void*>(material));
}

std::shared_ptr<RenderMesh> RenderResourceManager::GetEntityMesh(const RenderEntity& entity)
{
    std::shared_ptr<RenderMesh> cached_asset = MeshRegistry->GetAsset<RenderMesh>(entity.MeshAssetId);
    if (cached_asset)
    {
        return cached_asset;
    }

    throw std::runtime_error("failed to get entity render mesh");
}

std::shared_ptr<PBRMaterial> RenderResourceManager::GetEntityMaterial(const RenderEntity& entity)
{
    std::shared_ptr<PBRMaterial> cached_asset = MaterialRegistry->GetAsset<PBRMaterial>(entity.MaterialAssetId);
    if (cached_asset)
    {
        return cached_asset;
    }

    throw std::runtime_error("failed to get entity render material");
}

std::shared_ptr<TextureData> RenderResourceManager::LoadTexture(const String& file_path, bool is_srgb)
{
    std::shared_ptr<TextureData> cached_asset = DefaultRegistry->GetAsset<TextureData>(file_path);
    if (cached_asset)
    {
        return cached_asset;
    }

    if (file_path.empty())
        return nullptr;

    std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

    String            full_path = (GConfigManager->EngineRootPath / file_path).string();
    std::vector<byte> data      = FileManager::Read(full_path.c_str());

    int   channels;
    texture->Pixels = stbi_load_from_memory(
        data.data(), data.size(), (int*)&texture->Width, (int*)&texture->Height, &channels, STBI_rgb_alpha);
    if (!texture)
    {
        LOG_ERROR("failed to load texture: {}!", file_path);
        return nullptr;
    }

    texture->Depth     = 1;
    texture->ArrayLayers = 1;
    texture->MipLevels = std::floor(std::log2(std::max(texture->Width, texture->Height))) + 1;
    texture->Format      = is_srgb ? RHIFormat::eR8G8B8A8Srgb : RHIFormat::eR8G8B8A8Unorm;

    DefaultRegistry->Register(file_path, texture);

    return texture;
}

std::shared_ptr<TextureData> RenderResourceManager::LoadTextureHDR(const String& file_path, int desired_channels)
{
    std::shared_ptr<TextureData> cached_asset = DefaultRegistry->GetAsset<TextureData>(file_path);
    if (cached_asset)
    {
        return cached_asset;
    }

    if (file_path.empty())
        return nullptr;

    std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

    String            full_path = (GConfigManager->EngineRootPath / file_path).string();
    std::vector<byte> data      = FileManager::Read(full_path.c_str());

    int channels;
    texture->Pixels = stbi_load_from_memory(
        data.data(), data.size(), (int*)&texture->Width, (int*)&texture->Height, &channels, STBI_rgb_alpha);
    if (!texture)
    {
        throw std::runtime_error("failed to load texture!");
    }

    texture->Depth       = 1;
    texture->ArrayLayers = 1;
    texture->MipLevels   = std::floor(std::log2(std::max(texture->Width, texture->Height))) + 1;
    switch (desired_channels)
    {
        case 2:
            texture->Format = RHIFormat::eR32G32Sfloat;
            break;
        case 4:
            texture->Format = RHIFormat::eR32G32B32A32Sfloat;
            break;
        default:
            throw std::runtime_error("unsupported channels number");
            break;
    }

    DefaultRegistry->Register(file_path, texture);

    return texture;
}

std::shared_ptr<RenderMaterialData> RenderResourceManager::LoadMaterialData(const MaterialSourceDesc& material_res)
{
    std::shared_ptr<RenderMaterialData> material = std::make_shared<RenderMaterialData>();

    material->BaseColorTexture         = LoadTexture(material_res.BaseColorFile, true);
    material->NormalTexture            = LoadTexture(material_res.NormalFile);
    material->MetallicRoughnessTexture = LoadTexture(material_res.MetallicRoughnessFile);
    material->OcclusionTexture         = LoadTexture(material_res.OcclusionFile);
    material->EmissiveTexture          = LoadTexture(material_res.EmissiveFile);

    return material;
}

std::shared_ptr<RenderMeshData> RenderResourceManager::LoadMeshData(const MeshSourceDesc& mesh_source,
                                                                   AxisAlignedBox& bounding_box)
{
    std::shared_ptr<RenderMeshData> cached_asset = DefaultRegistry->GetAsset<RenderMeshData>(mesh_source.MeshFile);
    if (cached_asset)
    {
        return cached_asset;
    }

    if (mesh_source.MeshFile.empty())
        return nullptr;

    std::shared_ptr<RenderMeshData> mesh = std::make_shared<RenderMeshData>();
    mesh->MeshData                       = LoadStaticMesh(mesh_source.MeshFile, bounding_box);

    DefaultRegistry->Register(mesh_source.MeshFile, mesh);

    return mesh;
}

std::shared_ptr<RenderMesh> RenderResourceManager::GetOrCreateRenderMesh(const RenderEntity&   entity,
                                                                         const RenderMeshData& mesh_data)
{
    std::shared_ptr<RenderMesh> cached_asset = MeshRegistry->GetAsset<RenderMesh>(entity.MeshAssetId);
    if (cached_asset)
    {
        return cached_asset;
    }

    std::shared_ptr<RenderMesh> mesh = std::make_shared<RenderMesh>();
    mesh->MeshBoundingBox            = entity.BoundingBox;

    size_t index_buffer_size = mesh_data.MeshData.IndexBuffer->Size;
    byte*  index_buffer_data = mesh_data.MeshData.IndexBuffer->Data;

    size_t vertex_buffer_size = mesh_data.MeshData.VertexBuffer->Size;
    MeshVertexDataDefinition* vertex_buffer_data = (MeshVertexDataDefinition*)mesh_data.MeshData.VertexBuffer->Data;

    if (mesh_data.SkeletonBindingBuffer)
    {
        size_t joint_binding_buffer_size = mesh_data.MeshData.IndexBuffer->Size;
        MeshVertexBindingDataDefinition* joint_binding_buffer_data =
            (MeshVertexBindingDataDefinition*)mesh_data.MeshData.IndexBuffer->Data;
        UpdateMeshData(false,
                       index_buffer_size,
                       index_buffer_data,
                       vertex_buffer_size,
                       vertex_buffer_data,
                       joint_binding_buffer_size,
                       joint_binding_buffer_data,
                       *mesh);
    }
    else
    {
        UpdateMeshData(
            false, index_buffer_size, index_buffer_data, vertex_buffer_size, vertex_buffer_data, 0, nullptr, *mesh);
    }


    MeshRegistry->Register(entity.MeshAssetId, mesh);

    return mesh;
}

std::shared_ptr<PBRMaterial> RenderResourceManager::GetOrCreatePBRMaterial(const RenderEntity&       entity,
                                                                           const RenderMaterialData& material_data)
{
    std::shared_ptr<PBRMaterial> cached_asset = MaterialRegistry->GetAsset<PBRMaterial>(entity.MaterialAssetId);
    if (cached_asset)
    {
        return cached_asset;
    }

    std::shared_ptr<PBRMaterial> material = std::make_shared<PBRMaterial>();

    static byte empty_color[] {(byte)128, (byte)128, (byte)128, (byte)256};
    TextureData empty_texture {
        .Width  = 1,
        .Height = 1,
        .Pixels = empty_color,
        .Format = RHIFormat::eR8G8B8A8Unorm,
    };

    TextureDataToUpdate update_texture_data;
    update_texture_data.BaseColorTexture         = material_data.BaseColorTexture ? material_data.BaseColorTexture.get() : &empty_texture;
    update_texture_data.NormalTexture            = material_data.NormalTexture ? material_data.NormalTexture.get() : &empty_texture;
    update_texture_data.MetallicRoughnessTexture = material_data.MetallicRoughnessTexture ? material_data.MetallicRoughnessTexture.get() : &empty_texture;
    update_texture_data.OcclusionTexture         = material_data.OcclusionTexture ? material_data.OcclusionTexture.get() : &empty_texture;
    update_texture_data.EmissiveTexture          = material_data.EmissiveTexture ? material_data.EmissiveTexture.get() : &empty_texture;
    update_texture_data.Material                 = material.get();

    RHIDeviceSize buffer_size = sizeof(MeshPerMaterialUniformBufferObject);

    RHIBufferRef       inefficient_staging_buffer;
    RHIDeviceMemoryRef inefficient_staging_buffer_memory;
    void* inefficient_staging_buffer_data;
    RHI->CreateBuffer(buffer_size,
                      RHIBufferUsageFlagBits::eTransferSrc,
                      RHIMemoryPropertyFlagBits::eHostVisible | RHIMemoryPropertyFlagBits::eHostCoherent,
                      inefficient_staging_buffer,
                      inefficient_staging_buffer_memory);
    inefficient_staging_buffer_data = RHI->MapMemory(inefficient_staging_buffer_memory.get(), 0, buffer_size, {});

    MeshPerMaterialUniformBufferObject* material_uniform_buffer = (MeshPerMaterialUniformBufferObject*)inefficient_staging_buffer_data;
    // TODO

    RHI->UnmapMemory(inefficient_staging_buffer_memory.get());

    RHI->CreateBuffer(buffer_size,
                      RHIBufferUsageFlagBits::eUniformBuffer | RHIBufferUsageFlagBits::eTransferDst,
                      RHIMemoryPropertyFlagBits::eDeviceLocal,
                      material->UniformBuffer.BufferRHI,
                      material->UniformBuffer.DeviceMemoryRHI);

    RHI->CopyBuffer(inefficient_staging_buffer, material->UniformBuffer.BufferRHI, 0, 0, buffer_size);

    RHI->DestroyBuffer(inefficient_staging_buffer.get());
    RHI->FreeMemory(inefficient_staging_buffer_memory.get());


    UpdateTextureImageData(update_texture_data);

    RHIDescriptorSetAllocateInfo material_descriptor_set_alloc_info {
        .descriptorPool     = *(VulkanDescriptorPool*)RHI->GetDescriptorPool(),
        .descriptorSetCount = 1,
        .pSetLayouts        = (vk::DescriptorSetLayout*)PassCommon->MaterialLayout.get(),
    };

    material->DescriptorSet = RHIDescriptorSetRef(RHI->AllocateDescriptorSets(&material_descriptor_set_alloc_info));

    RHIDescriptorBufferInfo material_uniform_buffer_info = {
        .buffer = (vk::Buffer) * (VulkanBuffer*)material->UniformBuffer.BufferRHI.get(),
        .offset = 0,
        .range  = sizeof(MeshPerMaterialUniformBufferObject),
    };

    RHIDescriptorImageInfo base_color_image_info {
        .sampler     = *(VulkanSampler*)RHI->GetOrCreateMipmapSampler(update_texture_data.BaseColorTexture->Width, update_texture_data.BaseColorTexture->Height),
        .imageView   = *(VulkanImageView*)material->BaseColor->ImageViewRHI.get(),
        .imageLayout = RHIImageLayout::eShaderReadOnlyOptimal,
    };

    RHIDescriptorImageInfo metallic_roughness_image_info {
        .sampler     = *(VulkanSampler*)RHI->GetOrCreateMipmapSampler(update_texture_data.MetallicRoughnessTexture->Width, update_texture_data.MetallicRoughnessTexture->Height),
        .imageView   = *(VulkanImageView*)material->MetallicRoughness->ImageViewRHI.get(),
        .imageLayout = RHIImageLayout::eShaderReadOnlyOptimal,
    };

    RHIDescriptorImageInfo normal_image_info {
        .sampler     = *(VulkanSampler*)RHI->GetOrCreateMipmapSampler(update_texture_data.NormalTexture->Width, update_texture_data.NormalTexture->Height),
        .imageView   = *(VulkanImageView*)material->Normal->ImageViewRHI.get(),
        .imageLayout = RHIImageLayout::eShaderReadOnlyOptimal,
    };

    RHIDescriptorImageInfo occlusion_image_info {
        .sampler     = *(VulkanSampler*)RHI->GetOrCreateMipmapSampler(update_texture_data.OcclusionTexture->Width, update_texture_data.OcclusionTexture->Height),
        .imageView   = *(VulkanImageView*)material->Occlusion->ImageViewRHI.get(),
        .imageLayout = RHIImageLayout::eShaderReadOnlyOptimal,
    };

    RHIDescriptorImageInfo emissive_image_info {
        .sampler     = *(VulkanSampler*)RHI->GetOrCreateMipmapSampler(update_texture_data.EmissiveTexture->Width, update_texture_data.EmissiveTexture->Height),
        .imageView   = *(VulkanImageView*)material->Emissive->ImageViewRHI.get(),
        .imageLayout = RHIImageLayout::eShaderReadOnlyOptimal,
    };

    std::array<RHIWriteDescriptorSet, 6> mesh_descriptor_writes_info {};

    mesh_descriptor_writes_info[0].dstSet          = (vk::DescriptorSet) * (VulkanDescriptorSet*)material->DescriptorSet.get();
    mesh_descriptor_writes_info[0].dstBinding      = 0;
    mesh_descriptor_writes_info[0].dstArrayElement = 0;
    mesh_descriptor_writes_info[0].descriptorType  = RHIDescriptorType::eUniformBuffer;
    mesh_descriptor_writes_info[0].descriptorCount = 1;
    mesh_descriptor_writes_info[0].pBufferInfo     = &material_uniform_buffer_info;

    mesh_descriptor_writes_info[1].dstSet          = (vk::DescriptorSet) * (VulkanDescriptorSet*)material->DescriptorSet.get();
    mesh_descriptor_writes_info[1].dstBinding      = 1;
    mesh_descriptor_writes_info[1].dstArrayElement = 0;
    mesh_descriptor_writes_info[1].descriptorType  = RHIDescriptorType::eCombinedImageSampler;
    mesh_descriptor_writes_info[1].descriptorCount = 1;
    mesh_descriptor_writes_info[1].pImageInfo      = &base_color_image_info;

    mesh_descriptor_writes_info[2]                 = mesh_descriptor_writes_info[1];
    mesh_descriptor_writes_info[2].dstBinding      = 2;
    mesh_descriptor_writes_info[2].pImageInfo      = &metallic_roughness_image_info;

    mesh_descriptor_writes_info[3]                 = mesh_descriptor_writes_info[1];
    mesh_descriptor_writes_info[3].dstBinding      = 3;
    mesh_descriptor_writes_info[3].pImageInfo      = &normal_image_info;
    
    mesh_descriptor_writes_info[4]                 = mesh_descriptor_writes_info[1];
    mesh_descriptor_writes_info[4].dstBinding      = 4;
    mesh_descriptor_writes_info[4].pImageInfo      = &occlusion_image_info;
    
    mesh_descriptor_writes_info[5]                 = mesh_descriptor_writes_info[1];
    mesh_descriptor_writes_info[5].dstBinding      = 5;
    mesh_descriptor_writes_info[5].pImageInfo      = &occlusion_image_info;

    RHI->UpdateDescriptorSets(mesh_descriptor_writes_info, {});
    
    MaterialRegistry->Register(entity.MaterialAssetId, material);

    empty_texture.Pixels = nullptr;
    return material;
}

void RenderResourceManager::UpdateMeshData(bool                                   enable_vertex_blending,
                                           uint32_t                               index_buffer_size,
                                           void*                                  index_buffer_data,
                                           uint32_t                               vertex_buffer_size,
                                           MeshVertexDataDefinition const*        vertex_buffer_data,
                                           uint32_t                               joint_binding_buffer_size,
                                           MeshVertexBindingDataDefinition const* joint_binding_buffer_data,
                                           RenderMesh&                            mesh)
{
    mesh.EnableVertexBlending = enable_vertex_blending;

    ASSERT(0 == (vertex_buffer_size % sizeof(MeshVertexDataDefinition)));
    mesh.VertexCount = vertex_buffer_size / sizeof(MeshVertexDataDefinition);
    
    UpdateVertexBuffer(enable_vertex_blending,
                       vertex_buffer_size,
                       vertex_buffer_data,
                       joint_binding_buffer_size,
                       joint_binding_buffer_data,
                       index_buffer_size,
                       (uint16_t*)index_buffer_data,
                       mesh);

    ASSERT(0 == (index_buffer_size % sizeof(uint16_t)));
    mesh.IndexCount = index_buffer_size / sizeof(uint16_t);

    UpdateIndexBuffer(index_buffer_size, index_buffer_data, mesh);
}

void RenderResourceManager::UpdateVertexBuffer(bool                                   enable_vertex_blending,
                                               uint32_t                               vertex_buffer_size,
                                               MeshVertexDataDefinition const*        vertex_buffer_data,
                                               uint32_t                               joint_binding_buffer_size,
                                               MeshVertexBindingDataDefinition const* joint_binding_buffer_data,
                                               uint32_t                               index_buffer_size,
                                               uint16_t*                              index_buffer_data,
                                               RenderMesh&                            mesh)
{
    if (enable_vertex_blending)
    {
        ASSERT(0 == (vertex_buffer_size % sizeof(MeshVertexDataDefinition)));
        uint32_t vertex_count = vertex_buffer_size / sizeof(MeshVertexDataDefinition);
        ASSERT(0 == (index_buffer_size % sizeof(uint16_t)));
        uint32_t index_count = index_buffer_size / sizeof(uint16_t);

        RHIDeviceSize vertex_position_buffer_size                = sizeof(MeshVertex::MeshVertexPostition) * vertex_count;
        RHIDeviceSize vertex_varying_enable_blending_buffer_size = sizeof(MeshVertex::MeshVertexVaryingEnableBlending) * vertex_count;
        RHIDeviceSize vertex_varying_buffer_size                 = sizeof(MeshVertex::MeshVertexVarying) * vertex_count;
        RHIDeviceSize vertex_joint_binding_buffer_size           = sizeof(MeshVertex::MeshVertexJointBinding) * index_count;

        RHIDeviceSize vertex_position_buffer_offset                = 0;
        RHIDeviceSize vertex_varying_enable_blending_buffer_offset = vertex_position_buffer_offset + vertex_position_buffer_size;
        RHIDeviceSize vertex_varying_buffer_offset                 = vertex_varying_enable_blending_buffer_offset + vertex_varying_enable_blending_buffer_size;
        RHIDeviceSize vertex_joint_binding_buffer_offset           = vertex_varying_buffer_offset + vertex_varying_buffer_size;

        RHIDeviceSize      inefficient_staging_buffer_size = vertex_position_buffer_size + vertex_varying_enable_blending_buffer_size + vertex_varying_buffer_size + vertex_joint_binding_buffer_size;
        RHIBufferRef       inefficient_staging_buffer;
        RHIDeviceMemoryRef inefficient_staging_buffer_memory;
        RHI->CreateBuffer(inefficient_staging_buffer_size,
                          RHIBufferUsageFlagBits::eTransferSrc,
                          RHIMemoryPropertyFlagBits::eHostVisible | RHIMemoryPropertyFlagBits::eHostCoherent,
                          inefficient_staging_buffer,
                          inefficient_staging_buffer_memory);

        byte* inefficient_staging_buffer_data = (byte*)RHI->MapMemory(inefficient_staging_buffer_memory.get(), 0, RHI_WHOLE_SIZE, {});

        MeshVertex::MeshVertexPostition*             mesh_vertex_positions = (MeshVertex::MeshVertexPostition*)(inefficient_staging_buffer_data + vertex_position_buffer_offset);
        MeshVertex::MeshVertexVaryingEnableBlending* mesh_vertex_blending_varyings =
            (MeshVertex::MeshVertexVaryingEnableBlending*)(inefficient_staging_buffer_data + vertex_varying_enable_blending_buffer_offset);
        MeshVertex::MeshVertexVarying*      mesh_vertex_varyings      = (MeshVertex::MeshVertexVarying*)(inefficient_staging_buffer_data + vertex_varying_buffer_offset);
        MeshVertex::MeshVertexJointBinding* mesh_vertex_joint_binding = (MeshVertex::MeshVertexJointBinding*)(inefficient_staging_buffer_data + vertex_joint_binding_buffer_offset);

        for (uint32_t vertex_index = 0; vertex_index < vertex_count; vertex_index++)
        {
            mesh_vertex_positions[vertex_index].Position = vertex_buffer_data[vertex_index].Position;

            mesh_vertex_blending_varyings[vertex_index].Normal  = vertex_buffer_data[vertex_index].Normal;
            mesh_vertex_blending_varyings[vertex_index].Tangent = vertex_buffer_data[vertex_index].Tangent;

            mesh_vertex_varyings[vertex_index].Texcoord = vertex_buffer_data[vertex_index].TexCoord;
        }

        for (uint32_t index_index = 0; index_index < index_count; index_index++)
        {
            uint32_t vertex_buffer_index = index_buffer_data[index_index];

            mesh_vertex_joint_binding[index_index].Indices[0] = joint_binding_buffer_data[vertex_buffer_index].Index0;
            mesh_vertex_joint_binding[index_index].Indices[1] = joint_binding_buffer_data[vertex_buffer_index].Index1;
            mesh_vertex_joint_binding[index_index].Indices[2] = joint_binding_buffer_data[vertex_buffer_index].Index2;
            mesh_vertex_joint_binding[index_index].Indices[3] = joint_binding_buffer_data[vertex_buffer_index].Index3;

            float inv_total_weight = joint_binding_buffer_data[vertex_buffer_index].Weight0 + joint_binding_buffer_data[vertex_buffer_index].Weight1 +
                                     joint_binding_buffer_data[vertex_buffer_index].Weight2 + joint_binding_buffer_data[vertex_buffer_index].Weight3;

            inv_total_weight = (inv_total_weight != 0.0) ? 1 / inv_total_weight : 1.0;

            mesh_vertex_joint_binding[index_index].Weights = Vector4f(joint_binding_buffer_data[vertex_buffer_index].Weight0 * inv_total_weight,
                                                                      joint_binding_buffer_data[vertex_buffer_index].Weight1 * inv_total_weight,
                                                                      joint_binding_buffer_data[vertex_buffer_index].Weight2 * inv_total_weight,
                                                                      joint_binding_buffer_data[vertex_buffer_index].Weight3 * inv_total_weight);
        }

        RHI->UnmapMemory(inefficient_staging_buffer_memory.get());

        RHI->CreateBuffer(vertex_position_buffer_size,
                          RHIBufferUsageFlagBits::eVertexBuffer | RHIBufferUsageFlagBits::eTransferDst,
                          RHIMemoryPropertyFlagBits::eDeviceLocal,
                          mesh.VertexPositionBuffer.VertexBufferRHI,
                          mesh.VertexPositionBuffer.DeviceMemoryRHI);
        RHI->CreateBuffer(vertex_varying_enable_blending_buffer_size,
                          RHIBufferUsageFlagBits::eVertexBuffer | RHIBufferUsageFlagBits::eTransferDst,
                          RHIMemoryPropertyFlagBits::eDeviceLocal,
                          mesh.VertexVaryingEnableBlendingBuffer.VertexBufferRHI,
                          mesh.VertexVaryingEnableBlendingBuffer.DeviceMemoryRHI);
        RHI->CreateBuffer(vertex_varying_buffer_size,
                          RHIBufferUsageFlagBits::eVertexBuffer | RHIBufferUsageFlagBits::eTransferDst,
                          RHIMemoryPropertyFlagBits::eDeviceLocal,
                          mesh.VertexVaryingBuffer.VertexBufferRHI,
                          mesh.VertexVaryingBuffer.DeviceMemoryRHI);
        RHI->CreateBuffer(vertex_joint_binding_buffer_size,
                          RHIBufferUsageFlagBits::eStorageBuffer | RHIBufferUsageFlagBits::eTransferDst,
                          RHIMemoryPropertyFlagBits::eDeviceLocal,
                          mesh.VertexJointBindingBuffer.VertexBufferRHI,
                          mesh.VertexJointBindingBuffer.DeviceMemoryRHI);

        RHI->CopyBuffer(inefficient_staging_buffer, mesh.VertexPositionBuffer.VertexBufferRHI, vertex_position_buffer_offset, 0, vertex_position_buffer_size);
        RHI->CopyBuffer(
            inefficient_staging_buffer, mesh.VertexVaryingEnableBlendingBuffer.VertexBufferRHI, vertex_varying_enable_blending_buffer_offset, 0, vertex_varying_enable_blending_buffer_size);
        RHI->CopyBuffer(inefficient_staging_buffer, mesh.VertexVaryingBuffer.VertexBufferRHI, vertex_varying_buffer_offset, 0, vertex_varying_buffer_size);
        RHI->CopyBuffer(inefficient_staging_buffer, mesh.VertexJointBindingBuffer.VertexBufferRHI, vertex_joint_binding_buffer_offset, 0, vertex_joint_binding_buffer_size);

        RHI->DestroyBuffer(inefficient_staging_buffer.get());
        RHI->FreeMemory(inefficient_staging_buffer_memory.get());

        RHIDescriptorSetAllocateInfo mesh_vertex_blending_descriptor_set_alloc_info {
            .descriptorPool     = *(VulkanDescriptorPool*)RHI->GetDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts        = (vk::DescriptorSetLayout*)PassCommon->PerMeshLayout.get(),
        };

        mesh.VertexBlendingDescriptorSet = RHIDescriptorSetRef(RHI->AllocateDescriptorSets(&mesh_vertex_blending_descriptor_set_alloc_info));

        RHIDescriptorBufferInfo mesh_vertex_joint_binding_storage_buffer_info {
            .buffer = *(VulkanBuffer*)mesh.VertexJointBindingBuffer.VertexBufferRHI.get(),
            .offset = 0,
            .range  = vertex_joint_binding_buffer_size,
        };
        ASSERT(mesh_vertex_joint_binding_storage_buffer_info.range < PassCommon->GlobalRenderResource._StorageBuffers.MaxStorageBufferRange);

        RHIDescriptorSet* descriptor_set_to_write = mesh.VertexBlendingDescriptorSet.get();

        std::array<RHIWriteDescriptorSet, 1> descriptor_writes;

        RHIWriteDescriptorSet& mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info = descriptor_writes[0];
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.dstSet                 = (vk::DescriptorSet) * (VulkanDescriptorSet*)descriptor_set_to_write;
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.dstBinding             = 0;
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.dstArrayElement        = 0;
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.descriptorType         = RHIDescriptorType::eStorageBuffer;
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.descriptorCount        = 1;
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.pBufferInfo            = &mesh_vertex_joint_binding_storage_buffer_info;

        RHI->UpdateDescriptorSets(descriptor_writes, {});
    }
    else
    {
        ASSERT(0 == (vertex_buffer_size % sizeof(MeshVertexDataDefinition)));
        uint32_t vertex_count = vertex_buffer_size / sizeof(MeshVertexDataDefinition);

        RHIDeviceSize vertex_position_buffer_size                = sizeof(MeshVertex::MeshVertexPostition) * vertex_count;
        RHIDeviceSize vertex_varying_enable_blending_buffer_size = sizeof(MeshVertex::MeshVertexVaryingEnableBlending) * vertex_count;
        RHIDeviceSize vertex_varying_buffer_size                 = sizeof(MeshVertex::MeshVertexVarying) * vertex_count;

        RHIDeviceSize vertex_position_buffer_offset                = 0;
        RHIDeviceSize vertex_varying_enable_blending_buffer_offset = vertex_position_buffer_offset + vertex_position_buffer_size;
        RHIDeviceSize vertex_varying_buffer_offset                 = vertex_varying_enable_blending_buffer_offset + vertex_varying_enable_blending_buffer_size;

        RHIDeviceSize      inefficient_staging_buffer_size = vertex_position_buffer_size + vertex_varying_enable_blending_buffer_size + vertex_varying_buffer_size;
        RHIBufferRef       inefficient_staging_buffer;
        RHIDeviceMemoryRef inefficient_staging_buffer_memory;
        RHI->CreateBuffer(inefficient_staging_buffer_size,
                          RHIBufferUsageFlagBits::eTransferSrc,
                          RHIMemoryPropertyFlagBits::eHostVisible | RHIMemoryPropertyFlagBits::eHostCoherent,
                          inefficient_staging_buffer,
                          inefficient_staging_buffer_memory);

        byte* inefficient_staging_buffer_data = (byte*)RHI->MapMemory(inefficient_staging_buffer_memory.get(), 0, RHI_WHOLE_SIZE, {});

        MeshVertex::MeshVertexPostition*             mesh_vertex_positions = (MeshVertex::MeshVertexPostition*)(inefficient_staging_buffer_data + vertex_position_buffer_offset);
        MeshVertex::MeshVertexVaryingEnableBlending* mesh_vertex_blending_varyings =
            (MeshVertex::MeshVertexVaryingEnableBlending*)(inefficient_staging_buffer_data + vertex_varying_enable_blending_buffer_offset);
        MeshVertex::MeshVertexVarying* mesh_vertex_varyings = (MeshVertex::MeshVertexVarying*)(inefficient_staging_buffer_data + vertex_varying_buffer_offset);
        
        for (uint32_t vertex_index = 0; vertex_index < vertex_count; vertex_index++)
        {
            mesh_vertex_positions[vertex_index].Position = vertex_buffer_data[vertex_index].Position;

            mesh_vertex_blending_varyings[vertex_index].Normal = vertex_buffer_data[vertex_index].Normal;
             mesh_vertex_blending_varyings[vertex_index].Tangent = vertex_buffer_data[vertex_index].Tangent;

            mesh_vertex_varyings[vertex_index].Texcoord = vertex_buffer_data[vertex_index].TexCoord;
        }

        RHI->UnmapMemory(inefficient_staging_buffer_memory.get());


        RHI->CreateBuffer(vertex_position_buffer_size,
                          RHIBufferUsageFlagBits::eVertexBuffer | RHIBufferUsageFlagBits::eTransferDst,
                          RHIMemoryPropertyFlagBits::eDeviceLocal,
                          mesh.VertexPositionBuffer.VertexBufferRHI,
                          mesh.VertexPositionBuffer.DeviceMemoryRHI);
        RHI->CreateBuffer(vertex_varying_enable_blending_buffer_size,
                          RHIBufferUsageFlagBits::eVertexBuffer | RHIBufferUsageFlagBits::eTransferDst,
                          RHIMemoryPropertyFlagBits::eDeviceLocal,
                          mesh.VertexVaryingEnableBlendingBuffer.VertexBufferRHI,
                          mesh.VertexVaryingEnableBlendingBuffer.DeviceMemoryRHI);
        RHI->CreateBuffer(vertex_varying_buffer_size,
                          RHIBufferUsageFlagBits::eVertexBuffer | RHIBufferUsageFlagBits::eTransferDst,
                          RHIMemoryPropertyFlagBits::eDeviceLocal,
                          mesh.VertexVaryingBuffer.VertexBufferRHI,
                          mesh.VertexVaryingBuffer.DeviceMemoryRHI);

        RHI->CopyBuffer(inefficient_staging_buffer, mesh.VertexPositionBuffer.VertexBufferRHI, vertex_position_buffer_offset, 0, vertex_position_buffer_size);
        RHI->CopyBuffer(
            inefficient_staging_buffer, mesh.VertexVaryingEnableBlendingBuffer.VertexBufferRHI, vertex_varying_enable_blending_buffer_offset, 0, vertex_varying_enable_blending_buffer_size);
        RHI->CopyBuffer(inefficient_staging_buffer, mesh.VertexVaryingBuffer.VertexBufferRHI, vertex_varying_buffer_offset, 0, vertex_varying_buffer_size);

        RHI->DestroyBuffer(inefficient_staging_buffer.get());
        RHI->FreeMemory(inefficient_staging_buffer_memory.get());

        RHIDescriptorSetAllocateInfo mesh_vertex_blending_descriptor_set_alloc_info {
            .descriptorPool     = *(VulkanDescriptorPool*)RHI->GetDescriptorPool(),
            .descriptorSetCount = 1,
            .pSetLayouts        = (vk::DescriptorSetLayout*)PassCommon->PerMeshLayout.get(),
        };

        mesh.VertexBlendingDescriptorSet = RHIDescriptorSetRef(RHI->AllocateDescriptorSets(&mesh_vertex_blending_descriptor_set_alloc_info));

        RHIDescriptorBufferInfo mesh_vertex_joint_binding_storage_buffer_info {
            .buffer = *(VulkanBuffer*)PassCommon->GlobalRenderResource._StorageBuffers.NullStorageBuffer.BufferRHI.get(),
            .offset = 0,
            .range  = 1,
        };
        ASSERT(mesh_vertex_joint_binding_storage_buffer_info.range < PassCommon->GlobalRenderResource._StorageBuffers.MaxStorageBufferRange);

        RHIDescriptorSet* descriptor_set_to_write = mesh.VertexBlendingDescriptorSet.get();

        std::array<RHIWriteDescriptorSet, 1> descriptor_writes;

        RHIWriteDescriptorSet& mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info = descriptor_writes[0];
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.dstSet                 = (vk::DescriptorSet) * (VulkanDescriptorSet*)descriptor_set_to_write;
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.dstBinding             = 0;
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.dstArrayElement        = 0;
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.descriptorType         = RHIDescriptorType::eStorageBuffer;
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.descriptorCount        = 1;
        mesh_vertex_blending_vertex_joint_binding_storage_buffer_write_info.pBufferInfo            = &mesh_vertex_joint_binding_storage_buffer_info;

        RHI->UpdateDescriptorSets(descriptor_writes, {});
    }
}

void RenderResourceManager::UpdateIndexBuffer(uint32_t index_buffer_size, void* index_buffer_data, RenderMesh& mesh)
{
    RHIDeviceSize buffer_size = index_buffer_size;

    RHIBufferRef       inefficient_staging_buffer;
    RHIDeviceMemoryRef inefficient_staging_buffer_memory;
    RHI->CreateBuffer(buffer_size,
                      RHIBufferUsageFlagBits::eTransferSrc,
                      RHIMemoryPropertyFlagBits::eHostVisible | RHIMemoryPropertyFlagBits::eHostCoherent,
                      inefficient_staging_buffer,
                      inefficient_staging_buffer_memory);

    void* staging_buffer_data = RHI->MapMemory(inefficient_staging_buffer_memory.get(), 0, buffer_size, {});
    memcpy(staging_buffer_data, index_buffer_data, buffer_size);
    RHI->UnmapMemory(inefficient_staging_buffer_memory.get());

    // TODO: use vma?

    RHI->CreateBuffer(buffer_size,
                      RHIBufferUsageFlagBits::eIndexBuffer | RHIBufferUsageFlagBits::eTransferDst,
                      RHIMemoryPropertyFlagBits::eDeviceLocal,
                      mesh.IndexBuffer.IndexBufferRHI,
                      mesh.IndexBuffer.DeviceMemoryRHI);

    RHI->CopyBuffer(inefficient_staging_buffer.get(), mesh.IndexBuffer.IndexBufferRHI.get(), 0, 0, buffer_size);

    RHI->DestroyBuffer(inefficient_staging_buffer.get());
    RHI->FreeMemory(inefficient_staging_buffer_memory.get());
}

void RenderResourceManager::UpdateTextureImageData(const TextureDataToUpdate& texture_data)
{
    auto LoadGPUTexture = [this](TextureRef& texture, const TextureData* texture_data) {
        size_t     texture_hash   = (size_t)texture_data->Pixels;
        TextureRef cached_texture = TextureRegistry->GetAsset<Texture>(texture_hash);
        if (cached_texture)
        {
            texture = std::move(cached_texture);
            return;
        }

        texture = TextureRef(new Texture());
        texture->Format = texture_data->Format;
        RHI->CreateTextureImage(texture->ImageRHI, texture->ImageViewRHI, texture->DeviceMemoryRHI, texture_data);

        TextureRegistry->Register(texture_hash, texture);
    };

    LoadGPUTexture(texture_data.Material->BaseColor, texture_data.BaseColorTexture);
    LoadGPUTexture(texture_data.Material->Normal, texture_data.NormalTexture);
    LoadGPUTexture(texture_data.Material->MetallicRoughness, texture_data.MetallicRoughnessTexture);
    LoadGPUTexture(texture_data.Material->Occlusion, texture_data.OcclusionTexture);
    LoadGPUTexture(texture_data.Material->Emissive, texture_data.EmissiveTexture);
}

struct Model
{
    struct SubMesh
    {
        std::vector<MeshVertexDataDefinition> Vertices;
        std::vector<uint32_t>                 Indices;
        std::vector<String>                   Textures;
    };

    StringView           ModelName;
    bool                 FlipV {false};
    std::vector<SubMesh> Meshes;

    SubMesh MergeSubMesh()
    {
        SubMesh merged_mesh;

        uint32_t index_offset = 0;
        for (SubMesh& sub_mesh : Meshes)
        {
            merged_mesh.Vertices.insert(merged_mesh.Vertices.end(), sub_mesh.Vertices.begin(), sub_mesh.Vertices.end());

            for (uint32_t index : sub_mesh.Indices)
            {
                merged_mesh.Indices.push_back(index + index_offset);
            }

            index_offset += sub_mesh.Indices.size();

            // TODO: process texture
        }

        return merged_mesh;
    }
};

void ProcessModel(const aiScene* scene, const aiNode* node, Model* model);
void ProcessMesh(const aiScene* scene, const aiMesh* mesh, Model* model, Model::SubMesh* out_mesh);

StaticMeshData RenderResourceManager::LoadStaticMesh(const String& mesh_file, AxisAlignedBox& bounding_box)
{
    String            full_path = (GConfigManager->EngineRootPath / mesh_file).string();
    std::vector<byte> data      = FileManager::Read(full_path.c_str());

    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFileFromMemory(data.data(), data.size(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_ERROR("load static mesh '{}' failed, error: {}", mesh_file, importer.GetErrorString());
        return {};
    }

    StaticMeshData mesh_data;

    Model model;
    model.ModelName = mesh_file;
    model.FlipV  = mesh_file.ends_with(".obj");
    ProcessModel(scene, scene->mRootNode, &model);
    Model::SubMesh merged_mesh = model.MergeSubMesh();

    mesh_data.VertexBuffer = std::make_shared<BufferData>(merged_mesh.Vertices.size() * sizeof(MeshVertexDataDefinition));
    mesh_data.IndexBuffer  = std::make_shared<BufferData>(merged_mesh.Indices.size() * sizeof(uint16_t));

    ASSERT(merged_mesh.Vertices.size() <= std::numeric_limits<uint16_t>::max()); // check if need uint32_t 

    std::copy(merged_mesh.Vertices.begin(), merged_mesh.Vertices.end(), (MeshVertexDataDefinition*)mesh_data.VertexBuffer->Data);
    std::copy(merged_mesh.Indices.begin(), merged_mesh.Indices.end(), (uint16_t*)mesh_data.IndexBuffer->Data);

    for (auto& vertex : merged_mesh.Vertices)
    {
        bounding_box.extend(vertex.Position);
    }

    return mesh_data;
}

void ProcessModel(const aiScene* scene, const aiNode* node, Model* model)
{
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model->Meshes.push_back({});
        ProcessMesh(scene, mesh, model, &model->Meshes.back());
    }
    
    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessModel(scene, node->mChildren[i], model);
    }
}

void ProcessMesh(const aiScene* scene, const aiMesh* mesh, Model* model, Model::SubMesh* out_mesh)
{
    bool has_normal  = mesh->HasNormals();
    bool has_tangent = mesh->HasTangentsAndBitangents();
    bool has_texture = mesh->HasTextureCoords(0);

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        MeshVertexDataDefinition vertex;
        vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        if (has_normal)
        {
            vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        }
        if (has_tangent)
        {
            vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
        }
        if (has_texture)
        {
            if (model->FlipV)
            {
                vertex.TexCoord = {mesh->mTextureCoords[0][i].x, 1.0 - mesh->mTextureCoords[0][i].y};
            }
            else
            {
                vertex.TexCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
            }
        }

        out_mesh->Vertices.push_back(vertex);
    }

    if (!has_normal)
    {
        LOG_INFO("Generating normal for {}", model->ModelName);
    }
    if (!has_tangent)
    {
        LOG_INFO("Generating tangent for {}", model->ModelName);
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            out_mesh->Indices.push_back(face.mIndices[j]);
        }
        if (face.mNumIndices == 3 && (!has_normal || !has_tangent))
        {
            MeshVertexDataDefinition& vertex0 = out_mesh->Vertices[face.mIndices[0]];
            MeshVertexDataDefinition& vertex1 = out_mesh->Vertices[face.mIndices[1]];
            MeshVertexDataDefinition& vertex2 = out_mesh->Vertices[face.mIndices[2]];

            Vector3f e1 = vertex1.Position - vertex0.Position;
            Vector3f e2 = vertex2.Position - vertex1.Position;

            if (!has_normal)
            {
                Vector3f normal = e1.cross(e2).normalized();

                vertex0.Normal = normal;
                vertex1.Normal = normal;
                vertex2.Normal = normal;
            }
            if (!has_tangent)
            {
                Vector2f deltaUV1 = vertex1.TexCoord - vertex0.TexCoord;
                Vector2f deltaUV2 = vertex2.TexCoord - vertex1.TexCoord;

                auto divide = deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y();
                if (divide >= 0.0f && divide < 0.000001f)
                    divide = 0.000001f;
                else if (divide < 0.0f && divide > -0.000001f)
                    divide = -0.000001f;

                Vector3f tangent = (deltaUV2.y() * e1 - deltaUV1.y() * e2) / divide;
                tangent.normalize();

                vertex0.Tangent = tangent;
                vertex1.Tangent = tangent;
                vertex2.Tangent = tangent;
            }
        }
    }
}
