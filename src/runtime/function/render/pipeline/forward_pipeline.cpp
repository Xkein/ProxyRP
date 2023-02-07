#include "forward_pipeline.h"
#include "function/render/passes/directional_light_pass.h"
#include "function/render/passes/mesh_pass.h"
#include "function/render/passes/main_pass.h"
#include "function/render/passes/render_pass_common.h"
#include "function/render/render_scene.h"

void ForwardPipeline::Initialize(RenderPipelineInitInfo* init_info)
{
    PassCommon = std::make_shared<RenderPassCommon>();

    DirectionalLightPass = std::make_shared<DirectionalLightRenderPass>();
    MainPass             = std::make_shared<MainRenderPass>();
    MeshPass             = std::make_shared<MeshRenderPass>();

    RenderPassCommonInfo pass_common_info;
    pass_common_info.RHI        = RHI;
    pass_common_info.PassCommon = PassCommon;

    PassCommon->Initialize(&pass_common_info);

    DirectionalLightPass->SetCommonInfo(&pass_common_info);
    MainPass->SetCommonInfo(&pass_common_info);
    MeshPass->SetCommonInfo(&pass_common_info);

    DirectionalLightPassInitInfo directional_light_pass_init_info;
    DirectionalLightPass->Initialize(&directional_light_pass_init_info);

    MainPassInitInfo main_pass_init_info;
    main_pass_init_info.MeshPass = MeshPass;
    MainPass->Initialize(&main_pass_init_info);
    RHIRenderPassRef main_render_pass = static_cast<MainRenderPass*>(MainPass.get())->GetRenderPass();

    MeshPassInitInfo mesh_pass_init_info;
    mesh_pass_init_info.RenderPass                = main_render_pass;
    mesh_pass_init_info.DirectionalLightShadowMap = static_cast<DirectionalLightRenderPass*>(DirectionalLightPass.get())->GetDirectionalLightShadowMap();
    MeshPass->Initialize(&mesh_pass_init_info);

    DirectionalLightPass->PostInitialize();
    MainPass->PostInitialize();
    MeshPass->PostInitialize();
}

void ForwardPipeline::PreparePassData(RenderPipelinePrepareInfo* prepare_info)
{
    RenderPassPrepareInfo pass_prepare_info;
    pass_prepare_info.Scene           = prepare_info->Scene;
    pass_prepare_info.ResourceManager = prepare_info->ResourceManager;

    DirectionalLightPass->PrepareData(&pass_prepare_info);
    MainPass->PrepareData(&pass_prepare_info);
    MeshPass->PrepareData(&pass_prepare_info);
}

void ForwardPipeline::PassUpdateAfterRecreateSwapchain()
{
    FramebufferRecreateInfo recreate_info;

    DirectionalLightPass->UpdateAfterFramebufferRecreate(&recreate_info);
    MainPass->UpdateAfterFramebufferRecreate(&recreate_info);
    MeshPass->UpdateAfterFramebufferRecreate(&recreate_info);
}

void ForwardPipeline::Draw()
{

    DirectionalLightPass->Draw();

    MainPass->Draw();

}
