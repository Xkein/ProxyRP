#pragma once

#include "core/math/axis_aligned.h"
#include "function/render/render_swap_context.h"
#include "function/render/render_entity.h"
#include "function/render/render_type.h"
#include "platform/string.h"

class AssetRegistry;
struct MaterialResource;
struct MeshResource;
class VulkanRHI;

class RenderResourceManager final
{
    friend class RenderSystem;


public:
    void Clear();

    void UploadGlobalRenderResource(const LevelResourceDesc& level_resource_desc);
    void UploadGameObjectRenderResource(const RenderEntity&       render_entity,
                                        const RenderMeshData&     mesh_data,
                                        const RenderMaterialData& material_data);
    void UploadGameObjectRenderResource(const RenderEntity& render_entity, const RenderMeshData& mesh_data);
    void UploadGameObjectRenderResource(const RenderEntity& render_entity, const RenderMaterialData& material_data);

    std::shared_ptr<TextureData>        LoadTexture(const String& file_path, bool is_srgb = false);
    std::shared_ptr<TextureData>        LoadTextureHDR(const String& file_path, int desired_channels = 4);
    std::shared_ptr<RenderMaterialData> LoadMaterialData(const MaterialResource& material_res);
    std::shared_ptr<RenderMeshData> LoadMeshData(const MeshResource& mesh_res, const AxisAlignedBox& bounding_box = {});

    struct StorageBuffer
    {
        uint32_t UniformBufferAlignment {256};
        uint32_t StorageBufferAlignment {256};
        uint32_t MaxStorageBufferRange {1 << 27};

        RHIBufferRef Global
    };

    struct
    {
        StorageBuffer _StorageBuffer;
    } GlobalRenderResource;


private:
    StaticMeshData LoadStaticMesh(const String& mesh_file, const AxisAlignedBox& bounding_box);

    std::shared_ptr<RenderMesh> GetOrCreateRenderMesh(const RenderEntity& entity, const RenderMeshData& material_data);
    std::shared_ptr<PBRMaterial> GetOrCreatePBRMaterial(const RenderEntity& entity, const RenderMaterialData& material_data);

    void CreateAndMapStorageBuffer();

    std::shared_ptr<VulkanRHI>     RHI;
    std::shared_ptr<AssetRegistry> Registry;
};
