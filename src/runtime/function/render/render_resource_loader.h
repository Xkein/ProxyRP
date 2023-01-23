#pragma once

#include "core/math/axis_aligned.h"
#include "function/render/render_type.h"
#include "resource/res_type/data/mesh.h"
#include "resource/res_type/data/material.h"
#include "platform/string.h"

class AssetRegistry;

class RenderResourceLoader
{
public:

    std::shared_ptr<TextureData>        LoadTexture(const String& file_path, bool is_srgb = false);
    std::shared_ptr<TextureData>        LoadTextureHDR(const String& file_path, int desired_channels = 4);
    std::shared_ptr<RenderMaterialData> LoadMaterialData(const MaterialResource& material_res);
    std::shared_ptr<RenderMeshData>     LoadMeshData(const MeshResource& mesh_res, const AxisAlignedBox& bounding_box);

private:
    StaticMeshData LoadStaticMesh(const String& mesh_file, const AxisAlignedBox& bounding_box);

    std::shared_ptr<AssetRegistry> Registry;
};
