#include "render_pass_common.h"
#include "function/render/vulkan_rhi/vulkan_rhi.h"
#include "function/render/rhi_struct.h"

void RenderPassCommon::Initialize() {}

void RenderPassCommon::SetupDescriptorSetLayout()
{
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

    {
        std::array<RHIDescriptorSetLayoutBinding, 6> material_layout_bindings;

        RHIDescriptorSetLayoutBinding& material_uniform_buffer_binding = material_layout_bindings[0];
        material_uniform_buffer_binding.binding                        = 0;
        material_uniform_buffer_binding.descriptorType                 = RHIDescriptorType::eUniformBuffer;
        material_uniform_buffer_binding.descriptorCount                = 1;
        material_uniform_buffer_binding.stageFlags                     = RHIShaderStageFlagBits::eFragment;

        RHIDescriptorSetLayoutBinding& material_base_color_binding = material_layout_bindings[1];
        material_base_color_binding.binding                        = 1;
        material_base_color_binding.descriptorType                 = RHIDescriptorType::eCombinedImageSampler;
        material_base_color_binding.descriptorCount                = 1;
        material_base_color_binding.stageFlags                     = RHIShaderStageFlagBits::eFragment;

        RHIDescriptorSetLayoutBinding& material_metallic_roughness_binding = material_layout_bindings[2];
        material_metallic_roughness_binding                                = material_base_color_binding;
        material_metallic_roughness_binding.binding                        = 2;

        RHIDescriptorSetLayoutBinding& material_normal_binding = material_layout_bindings[3];
        material_normal_binding                                = material_base_color_binding;
        material_normal_binding.binding                        = 3;

        RHIDescriptorSetLayoutBinding& material_occlusion_binding = material_layout_bindings[4];
        material_occlusion_binding                                = material_base_color_binding;
        material_occlusion_binding.binding                        = 4;

        RHIDescriptorSetLayoutBinding& material_emissive_binding = material_layout_bindings[5];
        material_emissive_binding                                = material_base_color_binding;
        material_emissive_binding.binding                        = 5;

        RHIDescriptorSetLayoutCreateInfo layout_create_info {
            .bindingCount = material_layout_bindings.size(),
            .pBindings    = material_layout_bindings.data(),
        };

        MaterialLayout = RHIDescriptorSetLayoutRef(RHI->CreateDescriptorSetLayout(&layout_create_info));
    }
}


void RenderPassCommon::ResetRingBuffer(uint8_t current_frame_index)
{
    GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffersEnd[current_frame_index] =
        GlobalRenderResource._StorageBuffers.GlobalUploadRingBuffersBegin[current_frame_index];
}
void RenderPassCommon::CreateAndMapStorageBuffer()
{
    StorageBuffers& _storage_buffers = GlobalRenderResource._StorageBuffers;
    uint32_t        frames_in_flight = RHI->GetMaxFramesInFlight();

    RHIPhysicalDeviceProperties properties = RHI->GetPhysicalDeviceProperties();

    _storage_buffers.MinUniformBufferAlignment = properties.limits.minUniformBufferOffsetAlignment;
    _storage_buffers.MinStorageBufferAlignment = properties.limits.minStorageBufferOffsetAlignment;
    _storage_buffers.MaxStorageBufferRange     = properties.limits.maxStorageBufferRange;

    uint32_t global_storage_buffer_size = 1024 * 1024 * 128;

    RHI->CreateBuffer(global_storage_buffer_size,
                      RHIBufferUsageFlagBits::eStorageBuffer,
                      RHIMemoryPropertyFlagBits::eHostVisible | RHIMemoryPropertyFlagBits::eHostCoherent,
                      _storage_buffers.GlobalUploadRingBuffer.BufferRHI,
                      _storage_buffers.GlobalUploadRingBuffer.DeviceMemoryRHI);

    _storage_buffers.GlobalUploadRingBuffersBegin.resize(frames_in_flight);
    _storage_buffers.GlobalUploadRingBuffersEnd.resize(frames_in_flight);
    _storage_buffers.GlobalUploadRingBuffersSize.resize(frames_in_flight);

    for (uint32_t i = 0; i < frames_in_flight; i++)
    {
        _storage_buffers.GlobalUploadRingBuffersBegin[i] = (global_storage_buffer_size * i) / frames_in_flight;
        _storage_buffers.GlobalUploadRingBuffersSize[i]  = (global_storage_buffer_size * (i + 1)) / frames_in_flight -
                                                          (global_storage_buffer_size * i) / frames_in_flight;
    }

    _storage_buffers.GlobalUploadRingBufferMappedMemory =
        (byte*)RHI->MapMemory(_storage_buffers.GlobalUploadRingBuffer.DeviceMemoryRHI.get(), 0, RHI_WHOLE_SIZE, {});


    RHI->CreateBuffer(64, RHIBufferUsageFlagBits::eStorageBuffer, {}, _storage_buffers.NullStorageBuffer.BufferRHI, _storage_buffers.NullStorageBuffer.DeviceMemoryRHI);
}