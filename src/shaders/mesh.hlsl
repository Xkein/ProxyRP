
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

#define MeshPerframeBuffer _MeshPerframeBuffer[0]
StructuredBuffer<MeshPerframeStorageBufferObject> _MeshPerframeBuffer : register(t0);
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

void vert(float3 position : ATTRIBUTE0, float3 normal : ATTRIBUTE1, float3 tangent : ATTRIBUTE2, float2 texcoord : ATTRIBUTE3,
        uint vertex_index : SV_VertexID, uint instance_index : SV_InstanceID, out VS_OUTPUT output)
{
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
    
    float4 position_world_space = mul(model_matrix, float4(model_position, 1.0));
    position_world_space /= position_world_space.w;
    output.PositionWorldSpace = position_world_space.xyz;
    output.Position = mul(MeshPerframeBuffer.ProjViewMatrix, position_world_space);
    output.Normal = mul(model_matrix, float4(normal, 1.0)).xyz;
    output.Tangent = mul(model_matrix, float4(tangent, 1.0)).xyz;
    output.Texcoord = texcoord;
}

float3 CalculateNormal(float2 uv, float3 normal, float3 tangent)
{
    float3 tex_normal = NormalTexture.Sample(NormalTextureSampler, uv) * 2.0 - 1.0;
    float3 N = normalize(normal);
    float3 T = normalize(tangent);
    float3 B = cross(N, T);
    
    float3x3 TBN = float3x3(T, B, N);
    return normalize(mul(TBN, tex_normal));
}

float GetShadow(float2 uv, float cur_depth)
{
    float depth = DirectionalLightShadowMap.Sample(DirectionalLightShadowMapSampler, uv).x + 0.000075;
    return depth >= cur_depth ? 1.0 : 0.0;
}

float4 frag(VS_OUTPUT input) : SV_Target
{
    float3 base_color =  BaseColorTexture.Sample(BaseColorTextureSampler, input.Texcoord);
    float3 N = CalculateNormal(input.Texcoord, input.Normal, input.Tangent);
    float3 metallic_roughness = MetallicRoughnessTexture.Sample(MetallicRoughnessTextureSampler, input.Texcoord);
    float metallic = metallic_roughness.z;
    float roughness = metallic_roughness.y;
    const float3 dielectric_specular = float3(0.04, 0.04, 0.04);
    
    float3 V = normalize(MeshPerframeBuffer.CameraPosition - input.PositionWorldSpace);
    float3 R = reflect(-V, N);
    
    float3 F0 = lerp(dielectric_specular, base_color, float3(metallic, metallic, metallic));
    
    float3 Lo = (float3)0;
    
    int point_light_num = MeshPerframeBuffer.PointLightNum;
    for (int light_index = 0; light_index < point_light_num; ++light_index)
    {
        // TODO
    }

    {
        float3 L = normalize(MeshPerframeBuffer.DirectionalLight.Direction);
        float NoL = dot(N, L);
        
        if (NoL > 0.0)
        {
            float4 position_homo = mul(MeshPerframeBuffer.DirectionalLightProjView, float4(input.PositionWorldSpace, 1.0));
            float3 position_ndc = position_homo.xyz / position_homo.w;

            float2 shadow_uv = NDC2UV(position_ndc.xy);
            
            float shadow = GetShadow(shadow_uv, position_ndc.z);
            if (shadow > 0.0)
            {
                Lo += BRDF(N, V, L, F0, base_color, metallic, roughness) * MeshPerframeBuffer.DirectionalLight.Color * NoL * shadow;
            }
        }
    }
    
    Lo += base_color * MeshPerframeBuffer.AmbientLight;
    
    float3 Libl = (float3)0;
    Lo += Libl;
    
    return float4(Lo, 1.0);
}
