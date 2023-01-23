#include "forward_pipeline.h"
#include "function/render/passes/mesh_pass.h"

void ForwardPipeline::Initialize(RenderPipelineInitInfo* init_info)
{
    MeshPass = std::make_shared<::MeshPass>();

    RenderPassCommonInfo pass_common_info;
    pass_common_info.RHI = RHI;

    MeshPass->SetCommonInfo(pass_common_info);

    MeshPassInitInfo mesh_pass_init_info;
    MeshPass->Initialize(&mesh_pass_init_info);

    MeshPass->PostInitialize();
}

void ForwardPipeline::PreparePassData()
{
    MeshPass->PrepareData();
}

void ForwardPipeline::PassUpdateAfterRecreateSwapchain()
{
    MeshPass->UpdateAfterFramebufferRecreate();
}
