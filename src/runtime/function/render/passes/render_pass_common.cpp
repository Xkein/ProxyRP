#include "render_pass_common.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "function/render/rhi_struct.h"

void RenderPassCommon::Initialize() {}

void RenderPassCommon::SetupDescriptorSetLayout()
{

    std::array<RHIDescriptorSetLayoutBinding, 1> pre_mesh_layout_bindings;

    RHIDescriptorSetLayoutBinding& pre_mesh_uniform_buffer_binding = pre_mesh_layout_bindings[0];
    pre_mesh_uniform_buffer_binding.binding                        = 0;
    pre_mesh_uniform_buffer_binding.descriptorType                 = RHIDescriptorType::eStorageBuffer;
    pre_mesh_uniform_buffer_binding.descriptorCount                = 1;
    pre_mesh_uniform_buffer_binding.stageFlags                     = RHIShaderStageFlagBits::eVertex;

    RHIDescriptorSetLayoutCreateInfo layout_create_info {
        .bindingCount = pre_mesh_layout_bindings.size(),
        .pBindings    = pre_mesh_layout_bindings.data(),
    };

    PerMeshLayout = RHIDescriptorSetLayoutRef(RHI->CreateDescriptorSetLayout(&layout_create_info));
}
