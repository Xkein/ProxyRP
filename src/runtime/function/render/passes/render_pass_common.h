#pragma once

#include "function/render/render_pass.h"

class RenderPassCommon
{
public:
    void Initialize();

    void SetupDescriptorSetLayout();

    struct StorageBuffers
    {
        uint32_t MinUniformBufferAlignment {256};
        uint32_t MinStorageBufferAlignment {256};
        uint32_t MaxStorageBufferRange {1 << 27};

        StorageBuffer GlobalUploadRingBuffer;
        byte*         GlobalUploadRingBufferMappedMemory;

        std::vector<uint32_t> GlobalUploadRingBuffersBegin;
        std::vector<uint32_t> GlobalUploadRingBuffersEnd;
        std::vector<uint32_t> GlobalUploadRingBuffersSize;

        template<typename T>
        T* AllocateFromRingbuffer(uint8_t current_frame_index)
        {
            uint32_t offset = RoundUp(GlobalUploadRingBuffersEnd[current_frame_index], MinStorageBufferAlignment);
            GlobalUploadRingBuffersEnd[current_frame_index] = offset + sizeof(T);

            ASSERT(GlobalUploadRingBuffersEnd[current_frame_index] <=
                   GlobalUploadRingBuffersBegin[current_frame_index] +
                       GlobalUploadRingBuffersSize[current_frame_index]);

            return reinterpret_cast<T*>(GlobalUploadRingBufferMappedMemory + offset);
        }
    };

    struct
    {
        StorageBuffers _StorageBuffers;
    } GlobalRenderResource;

    void CreateAndMapStorageBuffer();
    void ResetRingBuffer(uint8_t current_frame_index);

    std::shared_ptr<VulkanRHI> RHI;

    RHIDescriptorSetLayoutRef PerMeshLayout;

};