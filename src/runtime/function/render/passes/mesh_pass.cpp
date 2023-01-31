#include "mesh_pass.h"

void MeshPass::Initialize(const RenderPassInitInfo* init_info)
{
    SetupAttachments();
    SetupRenderPass();
    SetupDescriptorSetLayout();
    SetupPipelines();
    SetupDescriptorSet();
}

void MeshPass::PostInitialize() {}

void MeshPass::SetCommonInfo(RenderPassCommonInfo* common_info) {}

void MeshPass::PrepareData(RenderPassPrepareInfo* prepare_info) {}

void MeshPass::UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info) {}

void MeshPass::Draw() {}


void MeshPass::SetupAttachments() {}

void MeshPass::SetupFramebuffer() {}

void MeshPass::SetupDescriptorSetLayout() {}

void MeshPass::SetupDescriptorSet() {}

void MeshPass::SetupRenderPass() {}

void MeshPass::SetupPipelines() {}

