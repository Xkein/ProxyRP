#include "mesh_pass.h"

void MeshPass::Initialize(const RenderPassInitInfo* init_info)
{
    SetupAttachments();
    SetupRenderPass();
    SetupDescriptorSetLayout();
    SetupPipelines();
    SetupDescriptorSet();
    SetupFramebufferDescriptorSet();
    SetupSwapchainFramebuffers();
}

void MeshPass::PrepareData() {}

void MeshPass::UpdateAfterFramebufferRecreate() {}

void MeshPass::Draw() {}

void MeshPass::SetupAttachments() {}

void MeshPass::SetupDescriptorSetLayout() {}

void MeshPass::SetupDescriptorSet() {}

void MeshPass::SetupFramebufferDescriptorSet() {}

void MeshPass::SetupRenderPass() {}

void MeshPass::SetupPipelines() {}

void MeshPass::SetupSwapchainFramebuffers() {}
