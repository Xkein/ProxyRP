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

    RHI = std::make_shared<VulkanRHI>();
    RHI->Initialize(rhi_init_info);

    ResourceManager = std::make_shared<RenderResourceManager>();
    ResourceManager->RHI = RHI;

    GlobalRenderingResource global_rendering_res;
    GAssetManager->LoadAsset(GConfigManager->Global.GlobalRenderingResourceUrl, global_rendering_res);

    __CompileAllShaders().wait();

    Camera = std::make_shared<RenderCamera>();

    Scene = std::make_shared<RenderScene>();
    Scene->ResourceManager = ResourceManager;

    Renderer = std::make_shared<ForwardSceneRenderer>();
    Renderer->RHI    = RHI;
    Renderer->Scene  = Scene;

    RenderPipelineInitInfo pipeline_init_info;
    Pipeline = std::static_pointer_cast<RenderPipeline>(std::make_shared<ForwardPipeline>());
    Pipeline->RHI = RHI;
    Pipeline->Initialize(&pipeline_init_info);

    ResourceManager->PassCommon = Pipeline->PassCommon;
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

    RenderPipelinePrepareInfo pipeline_prepare_info;
    pipeline_prepare_info.Scene           = Scene;
    pipeline_prepare_info.ResourceManager = ResourceManager;
    Pipeline->PreparePassData(&pipeline_prepare_info);

    Renderer->Render();
}

void RenderSystem::InitializeUIRenderBackend(WindowUI* window_ui)
{
    Pipeline->InitializeUIRenderBackend(window_ui);
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
        while (!swap_data.GameObjectToDelete->IsEmpty())
        {
            GameObjectDesc gobject = swap_data.GameObjectToDelete->PopAndGetNextProcessObject();

            for (size_t part_index = 0; part_index < gobject.GetObjectParts().size(); part_index++)
            {
                const GameObjectPartDesc& game_object_part = gobject.GetObjectParts()[part_index];
                GameObjectPartId          part_id          = {gobject.GetId(), part_index};

                bool is_entity_in_scene = Scene->GetInstanceIdAllocator().HasElement(part_id);

                RenderEntity render_entity;
                render_entity.InstanceId = 

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
