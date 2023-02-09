#include "render_system.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "function/render/render_camera.h"
#include "function/render/render_scene.h"
#include "function/render/render_resource_manager.h"
#include "function/render/pipeline/forward_pipeline.h"
#include "function/render/renderer/forward_renderer.h"
#include "function/global/global_context.h"
#include "resource/asset/asset_manager.h"
#include "resource/config/config_manager.h"
#include "resource/res_type/global/global_rendering.h"
#include "function/render/shader_compiler.h"

RenderSystem::~RenderSystem() {}

void RenderSystem::Initialize(RenderSystemInitInfo init_info)
{
    RHIInitInfo rhi_init_info;
    rhi_init_info.WindowSystem = init_info.WindowSystem;

    auto compiler_work = __CompileAllShaders();

    RHI = std::make_shared<VulkanRHI>();
    RHI->Initialize(rhi_init_info);

    ResourceManager = std::make_shared<RenderResourceManager>();
    ResourceManager->RHI = RHI;

    GlobalRenderingResource global_rendering_res;
    GAssetManager->LoadAsset(GConfigManager->Global.GlobalRenderingResourceUrl, global_rendering_res);

    LevelResourceDesc level_resource_desc;
    ResourceManager->UploadGlobalRenderResource(level_resource_desc);
    
    Camera = std::make_shared<RenderCamera>();
    const auto& camera_pose = global_rendering_res.CameraConfig.Pose;
    Camera->LookAt(camera_pose.Position, camera_pose.Target, camera_pose.Up);
    Camera->ZFar = global_rendering_res.CameraConfig.ZFar;
    Camera->ZNear = global_rendering_res.CameraConfig.ZNear;
    Camera->SetAspect(global_rendering_res.CameraConfig.Aspect.x() / global_rendering_res.CameraConfig.Aspect.y());
    Camera->SetMainViewMatrix();

    Scene = std::make_shared<RenderScene>();
    Scene->Light.Ambient               = {global_rendering_res.AmbientLight};
    Scene->Light.Directional.Direction = global_rendering_res.DirectionalLight.Direction.normalized();
    Scene->Light.Directional.Color     = global_rendering_res.DirectionalLight.Color;
    Scene->ResourceManager = ResourceManager;
    Scene->SetVisibleNodesReference();

    compiler_work.wait();

    Renderer = std::make_shared<ForwardSceneRenderer>();
    RendererInitInfo renderer_init_info;
    renderer_init_info.RHI = RHI;
    renderer_init_info.Scene = Scene;
    renderer_init_info.ResourceManager = ResourceManager;
    Renderer->Initialize(&renderer_init_info);

    ResourceManager->PassCommon = Renderer->Pipeline->PassCommon;
}

void RenderSystem::Clear()
{
    if (RHI)
    {
        RHI->Clear();
    }
    RHI.reset();

    if (ResourceManager)
    {
        ResourceManager->Clear();
    }
    ResourceManager.reset();

    if (Renderer)
    {
        Renderer->Clear();
    }
    Renderer.reset();
}

void RenderSystem::Tick(float delta_time)
{
    SwapLogicRenderData();

    ProcessSwapData();

    RHI->PrepareContext();

    Renderer->UpdatePerBuffer(Camera);

    Scene->UpdateVisibleObjects(Camera);

    Renderer->PrepareData();

    Renderer->Render();
}

void RenderSystem::InitializeUIRenderBackend(WindowUI* window_ui)
{
    Renderer->InitializeUIRenderBackend(window_ui);
}

void RenderSystem::SwapLogicRenderData()
{
    SwapContext.SwapLogicRenderData();
}

RenderSwapContext& RenderSystem::GetSwapContext()
{
    return SwapContext;
}

std::shared_ptr<RHI> RenderSystem::GetRHI() const
{
    return RHI;
}

std::shared_ptr<RenderCamera> RenderSystem::GetRenderCamera() const
{
    return Camera;
}

std::shared_ptr<RenderScene> RenderSystem::GetScene() const
{
    return Scene;
}

void RenderSystem::ProcessSwapData()
{
    RenderSwapData& swap_data = SwapContext.GetRenderSwapData();

    std::shared_ptr<AssetManager> asset_manager = GAssetManager;

    if (swap_data.LevelResource.has_value())
    {
        ResourceManager->UploadGlobalRenderResource(swap_data.LevelResource.value());

        // reset level resource swap data to a clean state
        SwapContext.ResetLevelRsourceSwapData();
    }

    // update game object if needed
    if (swap_data.GameObjectResource.has_value())
    {
        while (!swap_data.GameObjectResource->IsEmpty())
        {
            GameObjectDesc gobject = swap_data.GameObjectResource->PopAndGetNextProcessObject();

            for (size_t part_index = 0; part_index < gobject.GetObjectParts().size(); part_index++)
            {
                const GameObjectPartDesc& game_object_part = gobject.GetObjectParts()[part_index];
                GameObjectPartId          part_id          = {gobject.GetId(), part_index};

                bool is_entity_in_scene = Scene->GetInstanceIdAllocator().HasElement(part_id);

                RenderEntity render_entity;
                render_entity.InstanceId = Scene->GetInstanceIdAllocator().AllocGuid(part_id);
                render_entity.ModelMatrix = game_object_part.TransformDesc.TransformMatrix;
                 
                Scene->AddInstanceIdToMap(render_entity.InstanceId, gobject.GetId());

                // mesh properties
                MeshSourceDesc mesh_source    = {game_object_part.MeshDesc.MeshFile};
                bool           is_mesh_loaded = Scene->GetMeshAssetIdAllocator().HasElement(mesh_source);

                render_entity.MeshAssetId = Scene->GetMeshAssetIdAllocator().AllocGuid(mesh_source);

                std::shared_ptr<RenderMeshData> mesh_data;
                if (!is_mesh_loaded)
                {
                    mesh_data = ResourceManager->LoadMeshData(mesh_source, render_entity.BoundingBox);
                }
                else
                {
                    auto render_mesh = ResourceManager->GetEntityMesh(render_entity);
                    render_entity.BoundingBox = render_mesh->MeshBoundingBox;
                }

                render_entity.EnableVertexBlending = game_object_part.SkeletonAnimationResult.Transforms.size() > 1;
                render_entity.JointMatrices.resize(game_object_part.SkeletonAnimationResult.Transforms.size());

                for (size_t i = 0; i < game_object_part.SkeletonAnimationResult.Transforms.size(); i++)
                {
                    render_entity.JointMatrices[i] = game_object_part.SkeletonAnimationResult.Transforms[i].Matrix;
                }

                // material properties
                MaterialSourceDesc material_source;
                if (game_object_part.MaterialDesc.WithTexture)
                {
                    material_source.BaseColorFile         = game_object_part.MaterialDesc.BaseColorTexture;
                    material_source.NormalFile            = game_object_part.MaterialDesc.NormalTexture;
                    material_source.MetallicRoughnessFile = game_object_part.MaterialDesc.MetallicRoughnessTexture;
                    material_source.OcclusionFile         = game_object_part.MaterialDesc.OcclusionTexture;
                    material_source.EmissiveFile          = game_object_part.MaterialDesc.EmissiveTexture;
                }
                else
                {
                    material_source.BaseColorFile         = "asset/texture/default/albedo.jpg";
                    material_source.NormalFile            = "asset/texture/default/normal.jpg";
                    material_source.MetallicRoughnessFile = "asset/texture/default/mr.jpg";
                    material_source.OcclusionFile         = "";
                    material_source.EmissiveFile          = "";
                }

                bool is_material_load = Scene->GetMaterialAssetdAllocator().HasElement(material_source);

                render_entity.MaterialAssetId = Scene->GetMaterialAssetdAllocator().AllocGuid(material_source);

                std::shared_ptr<RenderMaterialData> material_data;
                if (!is_material_load)
                {
                    material_data = ResourceManager->LoadMaterialData(material_source);
                }


                //  create game object on the graphics api side
                if (!is_mesh_loaded)
                {
                    ResourceManager->UploadGameObjectRenderResource(render_entity, *mesh_data);
                }

                if (!is_material_load)
                {
                    ResourceManager->UploadGameObjectRenderResource(render_entity, *material_data);
                }

                // add object to render scene
                if (!is_entity_in_scene)
                {
                    Scene->RenderEntities.push_back(render_entity);
                }
                else
                {
                    for (auto& entity : Scene->RenderEntities)
                    {
                        if (entity.InstanceId == render_entity.InstanceId)
                        {
                            entity = render_entity;
                            break;
                        }
                    }
                }
            }
        }

        SwapContext.ResetGameObjectResourceSwapData();
    }

    // remove deleted objects
    if (swap_data.GameObjectToDelete.has_value())
    {
        while (!swap_data.GameObjectToDelete->IsEmpty())
        {
            GameObjectDesc gobject = swap_data.GameObjectToDelete->PopAndGetNextProcessObject();
            Scene->DeleteEntityByGObjectID(gobject.GetId());
        }

        SwapContext.ResetGameObjectToDelete();
    }

    // process camera swap data
    if (swap_data.CameraSwapData.has_value())
    {
        if (swap_data.CameraSwapData->FovX.has_value())
        {
            Camera->SetFovX(swap_data.CameraSwapData->FovX.value());
        }

        if (swap_data.CameraSwapData->ViewMatrix.has_value())
        {
            Camera->SetMainViewMatrix(swap_data.CameraSwapData->ViewMatrix.value());
        }

        SwapContext.ResetCameraSwapData();
    }
}
