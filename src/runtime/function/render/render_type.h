#pragma once

#include "rhi_definition.h"
#include "render_resource.h"
#include "core/math/vector.h"
#include "platform/platform.h"

#include <memory>
#include <cstdint>

class BufferData
{
public:
    byte*  Data;
    size_t Size;

    BufferData() = delete;
    BufferData(size_t size) : Size(size)
    {
        Data = (byte*)malloc(size);
    }
    ~BufferData()
    {
        Clear();
    }
    void Clear()
    {
        if (Data)
        {
            free(Data);
        }
    }
    operator bool() const
    {
        return Data;
    }
};

class TextureData
{
public:
    uint32_t Width;
    uint32_t Height;
    uint32_t Depth;
    uint32_t MipLevels {0};
    uint32_t ArrayLayers {0};
    byte*    Pixels {nullptr};

    RHIFormat Format;

    ~TextureData()
    {
        free(this->Pixels);
    }

    operator bool() const
    {
        return Pixels;
    }
};

struct MeshVertexData
{
    Vector3f Position;
    Vector3f Normal;
    Vector2f TexCoords;
};

struct StaticMeshData
{
    std::shared_ptr<BufferData> VertexBuffer;
    std::shared_ptr<BufferData> IndexBuffer;
};

struct RenderMeshData
{
    StaticMeshData              MeshData;
    std::shared_ptr<BufferData> SkeletonBindingBuffer;
};

struct RenderMaterialData
{
    std::shared_ptr<TextureData> BaseColorTexture;
    std::shared_ptr<TextureData> NormalTexture;
    std::shared_ptr<TextureData> MetallicRoughnessTexture;
    std::shared_ptr<TextureData> OcclusionTexture;
    std::shared_ptr<TextureData> EmissiveTexture;
};


struct RenderMesh
{
    bool EnableVertexBlending;

    uint32_t VertexCount;

    VertexBufferRef PositionBuffer;
    VertexBufferRef VaryingEnableBlendingBuffer;
    VertexBufferRef JointBindingBuffer;

    RHIDescriptorSetRef BlendingDescriptorSet;

    VertexBufferRef VaryingBuffer;

    uint32_t IndexCount;

    IndexBufferRef IndexBuffer;
};

struct PBRMaterial
{
    TextureRef BaseColor;
    TextureRef MetallicRoughness;
    TextureRef Normal;
    TextureRef Occlusion;
    TextureRef Emissive;

    RHIDescriptorSetRef DescriptorSet;
};
