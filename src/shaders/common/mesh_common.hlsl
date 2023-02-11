#pragma once

#include "structures.hlsl"

#define MeshPerframeBuffer _MeshPerframeBuffer[0]
StructuredBuffer<MeshPerframeStorageBufferObject> _MeshPerframeBuffer : register(t0);
StructuredBuffer<MeshInstance> MeshInstances : register(t1);
StructuredBuffer<float4x4> JointMatrices : register(t2);

StructuredBuffer<MeshVertexJointBinding> JointBindings : register(t0, space1);

Texture2D BaseColorTexture : register(t1, space2);
SamplerState BaseColorTextureSampler : register(s1, space2);
Texture2D MetallicRoughnessTexture : register(t2, space2);
SamplerState MetallicRoughnessTextureSampler : register(s2, space2);
Texture2D NormalTexture : register(t3, space2);
SamplerState NormalTextureSampler : register(s3, space2);
Texture2D OcclusionTexture : register(t4, space2);
SamplerState OcclusionTextureSampler : register(s4, space2);
Texture2D EmissiveTexture : register(t5, space2);
SamplerState EmissiveTextureSampler : register(s5, space2);

float3 GetBaseColor(float2 uv)
{
    float3 color = BaseColorTexture.Sample(BaseColorTextureSampler, uv).xyz;
    //return color;
    // convert to linear space
    return pow(color, 2.2);
}

void GetPosition(float4x4 model_matrix, float4x4 proj_view_matrix, float3 model_position, out float3 position_world_space, out float4 position_mvp)
{
    float4 tmp = mul(model_matrix, float4(model_position, 1.0));
    position_world_space = tmp.xyz;
    position_mvp = mul(proj_view_matrix, float4(position_world_space, 1.0));
}

float3x3 CalcTangentToLocal(float3 normal, float3 tangent)
{
    float3 binormal = cross(normal, tangent);
    
    float3x3 TBN;
    TBN[0] = cross(binormal, normal);
    TBN[1] = binormal;
    TBN[2] = normal;
    return TBN;
}

float3 CalculateNormal(float2 uv, float3 normal, float3 tangent)
{
    float3 tex_normal = NormalTexture.Sample(NormalTextureSampler, uv).xyz * 2.0 - 1.0;
    float3x3 TBN = CalcTangentToLocal(normal, tangent);
    return normalize(mul(tex_normal, TBN));
}

void GetMetallicRoughness(float2 uv, out float metallic, out float roughness)
{
    float3 metallic_roughness = MetallicRoughnessTexture.Sample(MetallicRoughnessTextureSampler, uv).xyz;
    metallic = metallic_roughness.z;
    roughness = metallic_roughness.y;
}

void GetMeshBlendingResult(float3 position, float3 normal, float3 tangent, float enable_vertex_blending, uint vertex_index, uint instance_index,
                        out float3 model_position, out float3 model_normal, out float3 model_tangent)
{
    if (enable_vertex_blending > 0.0)
    {
        // TODO
    }
    else
    {
        model_position = position;
        model_normal = normal;
        model_tangent = tangent;
    }
}
