#include "forward_pipeline.h"
#include "function/render/passes/directional_light_pass.h"
#include "function/render/passes/mesh_pass.h"
#include "function/render/passes/render_pass_common.h"
#include "function/render/render_scene.h"

void ForwardPipeline::Initialize(RenderPipelineInitInfo* init_info)
{
    PassCommon = std::make_shared<RenderPassCommon>();

    DirectionalLightPass = std::make_shared<::DirectionalLightPass>();
    MeshPass             = std::make_shared<::MeshPass>();

    RenderPassCommonInfo pass_common_info;
    pass_common_info.RHI        = RHI;
    pass_common_info.PassCommon = PassCommon;

    PassCommon->Initialize(&pass_common_info);

    DirectionalLightPass->SetCommonInfo(&pass_common_info);
    MeshPass->SetCommonInfo(&pass_common_info);

    DirectionalLightPassInitInfo directional_light_pass_init_info;
    DirectionalLightPass->Initialize(&directional_light_pass_init_info);

    MeshPassInitInfo mesh_pass_init_info;
    mesh_pass_init_info.DirectionalLightShadowMap = static_cast<::DirectionalLightPass*>(DirectionalLightPass.get())->GetDirectionalLightShadowMap();
    MeshPass->Initialize(&mesh_pass_init_info);

    DirectionalLightPass->PostInitialize();
    MeshPass->PostInitialize();
}

void ForwardPipeline::PreparePassData(RenderPipelinePrepareInfo* prepare_info)
{
    RenderPassPrepareInfo pass_prepare_info;
    pass_prepare_info.Scene           = prepare_info->Scene;
    pass_prepare_info.ResourceManager = prepare_info->ResourceManager;

    DirectionalLightPass->PrepareData(&pass_prepare_info);
    MeshPass->PrepareData(&pass_prepare_info);
}

void ForwardPipeline::PassUpdateAfterRecreateSwapchain()
{
    FramebufferRecreateInfo recreate_info;

    DirectionalLightPass->UpdateAfterFramebufferRecreate(&recreate_info);
    MeshPass->UpdateAfterFramebufferRecreate(&recreate_info);
}

void ForwardPipeline::Draw()
{

    DirectionalLightPass->Draw();

    MeshPass->Draw();


}
