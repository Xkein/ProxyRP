#include "mesh_component.h"
#include "resource/asset/asset_manager.h"
#include "function/render/render_system.h"
#include "function/global/global_context.h"
#include "function/framework/object/game_object.h"
#include "function/framework/component/transform/transform_component.h"
#include "function/framework/component/animation/animation_component.h"

void MeshComponent::PostLoadResource(std::weak_ptr<GameObject> parent_object)
{
    Component::PostLoadResource(parent_object);

    RawMeshes.reserve(MeshRes.SubMeshes.size());

    for (size_t index = 0; index < MeshRes.SubMeshes.size(); index++)
    {
        LoadMeshResource(MeshRes.SubMeshes[index]);
    }

}

void MeshComponent::AddMeshResource(SubMeshResource sub_mesh_res) {
    LoadMeshResource(MeshRes.SubMeshes.emplace_back(std::move(sub_mesh_res)));
}


void MeshComponent::LoadMeshResource(const SubMeshResource& sub_mesh_res)
{
    std::shared_ptr<AssetManager> asset_manager  = GAssetManager;
    GameObjectPartDesc& mesh_part_desc = RawMeshes.emplace_back();

    mesh_part_desc.MeshDesc.MeshFile        = sub_mesh_res.MeshFileRef;
    mesh_part_desc.MaterialDesc.WithTexture = !sub_mesh_res.Material.empty();

    if (mesh_part_desc.MaterialDesc.WithTexture)
    {
        MaterialResource material_res;
        asset_manager->LoadAsset(sub_mesh_res.Material, material_res);

        mesh_part_desc.MaterialDesc.BaseColorTexture         = material_res.BaseColorFile;
        mesh_part_desc.MaterialDesc.NormalTexture            = material_res.NormalFile;
        mesh_part_desc.MaterialDesc.MetallicRoughnessTexture = material_res.MetallicRoughnessFile;
        mesh_part_desc.MaterialDesc.OcclusionTexture         = material_res.OcclusionFile;
        mesh_part_desc.MaterialDesc.EmissiveTexture          = material_res.EmissiveFile;
    }

    Matrix4x4 object_space_transform             = sub_mesh_res.Transform.GetMatrix();
    mesh_part_desc.TransformDesc.TransformMatrix = object_space_transform;
}

void MeshComponent::Tick(float delta_time)
{
    std::shared_ptr<GameObject> parent_object = ParentObject.lock();

    TransformComponent* transform_component = parent_object->GetComponent<TransformComponent>();
    const AnimationComponent* animation_component = parent_object->GetComponent<AnimationComponent>();

    if (transform_component->IsDirty())
    {
        std::vector<GameObjectPartDesc> dirty_mesh_parts;
        SkeletonAnimationResult         animation_result;
        animation_result.Transforms.push_back({Matrix4x4::Identity()});
        if (animation_component)
        {

        }
        for (GameObjectPartDesc& mesh_part : RawMeshes)
        {
            if (animation_component)
            {

            }
            Matrix4x4 object_transform_matrix = mesh_part.TransformDesc.TransformMatrix;

            mesh_part.TransformDesc.TransformMatrix = transform_component->GetMatrix() * object_transform_matrix;

            dirty_mesh_parts.push_back(mesh_part);

            mesh_part.TransformDesc.TransformMatrix = object_transform_matrix;
        }

        RenderSwapContext& render_swap_context = GRenderSystem->GetSwapContext();
        RenderSwapData&    logic_swap_data     = render_swap_context.GetLogicSwapData();

        logic_swap_data.AddDirtyGameObject(GameObjectDesc {parent_object->GetID(), dirty_mesh_parts});

        transform_component->SetDirtyFlag(false);
    }

}
