#pragma once

#include "core/math/axis_aligned.h"
#include "function/render/render_swap_context.h"
#include "function/render/render_entity.h"
#include "function/render/render_type.h"
#include "platform/string.h"

class AssetRegistry;
class RHI;

class RenderResourceManager final
{
    friend class RenderSystem;


public:
    RenderResourceManager();

    void Clear();

    void UploadGlobalRenderResource(const LevelResourceDesc& level_resource_desc);
    void UploadGameObjectRenderResource(const RenderEntity&       render_entity,
                                        const RenderMeshData&     mesh_data,
                                        const RenderMaterialData& material_data);
    void UploadGameObjectRenderResource(const RenderEntity& render_entity, const RenderMeshData& mesh_data);
    void UploadGameObjectRenderResource(const RenderEntity& render_entity, const RenderMaterialData& material_data);

    std::shared_ptr<RenderMesh> GetEntityMesh(const RenderEntity& entity);
    std::shared_ptr<PBRMaterial> GetEntityMaterial(const RenderEntity& entity);

    std::shared_ptr<TextureData>        LoadTexture(const String& file_path, bool is_srgb = false);
    std::shared_ptr<TextureData>        LoadTextureHDR(const String& file_path, int desired_channels = 4);
    std::shared_ptr<RenderMaterialData> LoadMaterialData(const MaterialSourceDesc& material_res);
    std::shared_ptr<RenderMeshData>     LoadMeshData(const MeshSourceDesc& mesh_source, AxisAlignedBox& bounding_box);

private:
    StaticMeshData LoadStaticMesh(const String& mesh_file, AxisAlignedBox& bounding_box);

    std::shared_ptr<RenderMesh> GetOrCreateRenderMesh(const RenderEntity& entity, const RenderMeshData& mesh_data);
    std::shared_ptr<PBRMaterial> GetOrCreatePBRMaterial(const RenderEntity& entity, const RenderMaterialData& material_data);
    
    void UpdateMeshData(bool                                          enable_vertex_blending,
                        uint32_t                                      index_buffer_size,
                        void*                                         index_buffer_data,
                        uint32_t                                      vertex_buffer_size,
                        struct MeshVertexDataDefinition const*        vertex_buffer_data,
                        uint32_t                                      joint_binding_buffer_size,
                        struct MeshVertexBindingDataDefinition const* joint_binding_buffer_data,
                        RenderMesh&                                   mesh);
    void UpdateVertexBuffer(bool                                          enable_vertex_blending,
                            uint32_t                                      vertex_buffer_size,
                            struct MeshVertexDataDefinition const*        vertex_buffer_data,
                            uint32_t                                      joint_binding_buffer_size,
                            struct MeshVertexBindingDataDefinition const* joint_binding_buffer_data,
                            uint32_t                                      index_buffer_size,
                            uint16_t*                                     index_buffer_data,
                            RenderMesh&                                   mesh);
    void UpdateIndexBuffer(uint32_t index_buffer_size, void* index_buffer_data, RenderMesh& mesh);

    void UpdateTextureImageData(const TextureDataToUpdate& texture_data);

    std::shared_ptr<RHI>     RHI;

    std::shared_ptr<AssetRegistry> DefaultRegistry;
    std::shared_ptr<AssetRegistry> MeshRegistry;
    std::shared_ptr<AssetRegistry> TextureRegistry;
    std::shared_ptr<AssetRegistry> MaterialRegistry;

    std::vector<std::shared_ptr<void*>> __PersistentResources;

    std::shared_ptr<class RenderPassCommon>   PassCommon;
};
