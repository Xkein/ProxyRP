#pragma once

#include "core/math/math.h"
#include "core/macro.h"
#include "function/render/render_pass.h"

class RenderPassCommon
{
public:
    void Initialize(const RenderPassCommonInfo* common_info);

    void Clear();

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

        StorageBuffer NullStorageBuffer;

        template<typename T>
        T* AllocateFromRingbuffer(uint8_t current_frame_index, uint32_t& offset)
        {
            offset = RoundUp(GlobalUploadRingBuffersEnd[current_frame_index], MinStorageBufferAlignment);
            GlobalUploadRingBuffersEnd[current_frame_index] = offset + sizeof(T);

            ASSERT(GlobalUploadRingBuffersEnd[current_frame_index] <=
                   GlobalUploadRingBuffersBegin[current_frame_index] +
                       GlobalUploadRingBuffersSize[current_frame_index]);

            return reinterpret_cast<T*>(GlobalUploadRingBufferMappedMemory + offset);
        }
    };

    struct IBLResource
    {
        Texture       BrdfLUTTexture;
        RHISamplerRef BrdfLUTTextureSampler;

        Texture       IrradianceTexture;
        RHISamplerRef IrradianceTextureSampler;

        Texture       SpecularTexture;
        RHISamplerRef SpecularTextureSampler;
    };

    struct
    {
        IBLResource    _IBLResource;
        StorageBuffers _StorageBuffers;
    } GlobalRenderResource;

    void CreateAndMapStorageBuffer();
    void ResetRingBuffer(uint8_t current_frame_index);

    void CreateIBLSamplers();
    void CreateIBLTextures(std::array<std::shared_ptr<TextureData>, 6> irradiance_maps, std::array<std::shared_ptr<TextureData>, 6> specular_maps);

    std::shared_ptr<RHI> RHI;

    RHIDescriptorSetLayoutRef PerMeshLayout;
    RHIDescriptorSetLayoutRef MaterialLayout;

};
