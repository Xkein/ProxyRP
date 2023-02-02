
#include "common/constants.hlsl"
#include "common/structures.hlsl"
#include "common/mesh_shading.hlsl"

float2 NDC2UV(float2 xy)
{
    return xy * float2(0.5, 0.5) + float2(0.5, 0.5);
}
float2 UV2NDC(float2 uv)
{
    return uv * float2(2.0, 2.0) + float2(-1.0, -1.0);
}


StructuredBuffer<MeshPerframeStorageBufferObject> MeshPerframeBuffer : register(t0);
StructuredBuffer<MeshInstance> MeshInstances : register(t1);
StructuredBuffer<float4x4> JointMatrices : register(t2);

StructuredBuffer<MeshVertexJointBinding> JointBindings : register(t0, space1);

Texture2DArray PointLightShadowMap        : register(t3);
SamplerState   PointLightShadowMapSampler : register(s3);

Texture2D    DirectionalLightShadowMap        : register(t4);
SamplerState DirectionalLightShadowMapSampler : register(s4);

Texture2D    BaseColorTexture        : register(t1, space2);
SamplerState BaseColorTextureSampler : register(s1, space2);
Texture2D    MetallicRoughnessTexture        : register(t2, space2);
SamplerState MetallicRoughnessTextureSampler : register(s2, space2);
Texture2D    NormalTexture        : register(t3, space2);
SamplerState NormalTextureSampler : register(s3, space2);
Texture2D    OcclusionTexture        : register(t4, space2);
SamplerState OcclusionTextureSampler : register(s4, space2);
Texture2D    EmissiveTexture        : register(t5, space2);
SamplerState EmissiveTextureSampler : register(s5, space2);

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 PositionWorldSpace : Position;
    float3 Normal : TEXCOORD0;
    float3 Tangent : TEXCOORD1;
    float2 Texcoord : TEXCOORD2;
};

VS_OUTPUT vert(float3 position : ATTRIBUTE0, float3 normal : ATTRIBUTE1, float3 tangent : ATTRIBUTE2, float2 texcoord : ATTRIBUTE3, uint vertex_index : SV_VertexID, uint instance_index : SV_InstanceID)
{
    VS_OUTPUT output;
    
    float4x4 model_matrix = MeshInstances[instance_index].ModelMatrix;
    float enable_vertex_blending = MeshInstances[instance_index].EnableVertexBlending;
    
    float3 model_position;
    float3 model_normal;
    float3 model_tangent;
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
    
    float3 position_world_space = mul(model_matrix, float4(model_position, 1.0)).xyz;
    output.PositionWorldSpace = position_world_space;
    output.Position = mul(MeshPerframeBuffer[0].ProjViewMatrix, float4(position_world_space, 1.0));
    output.Normal = mul(model_matrix, float4(normal, 1.0)).xyz;
    output.Tangent = mul(model_matrix, float4(tangent, 1.0)).xyz;
    output.Texcoord = texcoord;
    
    return output;
}

float4 frag(VS_OUTPUT input) : SV_Target
{
    return BaseColorTexture.Sample(BaseColorTextureSampler, input.Texcoord);
}
