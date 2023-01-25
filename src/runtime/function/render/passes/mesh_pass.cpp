#include "mesh_pass.h"

void MeshPass::Initialize(const RenderPassInitInfo* init_info)
{
    SetupAttachments();
    SetupRenderPass();
    SetupDescriptorSetLayout();
    SetupPipelines();
    SetupDescriptorSet();
}

void MeshPass::PrepareData() {}

void MeshPass::UpdateAfterFramebufferRecreate(const FramebufferRecreateInfo* recreate_info) {}

void MeshPass::Draw() {}


void MeshPass::SetupAttachments() {}

void MeshPass::SetupDescriptorSetLayout() {}

void MeshPass::SetupDescriptorSet() {}

void MeshPass::SetupRenderPass() {}

void MeshPass::SetupPipelines() {}

