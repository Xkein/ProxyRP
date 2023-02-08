
#include "common/constants.hlsl"
#include "common/structures.hlsl"
#include "common/mesh_shading.hlsl"
#include "common/mesh_common.hlsl"

float2 NDC2UV(float2 xy)
{
    return xy * float2(0.5, 0.5) + float2(0.5, 0.5);
}
float2 UV2NDC(float2 uv)
{
    return uv * float2(2.0, 2.0) + float2(-1.0, -1.0);
}

Texture2DArray PointLightShadowMap        : register(t3);
SamplerState   PointLightShadowMapSampler : register(s3);

Texture2D    DirectionalLightShadowMap        : register(t4);
SamplerState DirectionalLightShadowMapSampler : register(s4);


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
    GetMeshBlendingResult(position, normal, tangent, enable_vertex_blending, vertex_index, instance_index, model_position, model_normal, model_tangent);
    
    GetPosition(model_matrix, MeshPerframeBuffer.ProjViewMatrix, model_position, output.PositionWorldSpace, output.Position);
    float3x3 tangent_matrix = float3x3(model_matrix[0].xyz, model_matrix[1].xyz, model_matrix[2].xyz);
    output.Normal = mul(tangent_matrix, model_normal);
    output.Tangent = mul(tangent_matrix, model_tangent);
    output.Texcoord = texcoord;
}

float GetShadow(float2 uv, float cur_depth)
{
    float depth = DirectionalLightShadowMap.Sample(DirectionalLightShadowMapSampler, uv).x + 0.0075;
    return depth >= cur_depth ? 1.0 : -1.0;
}

float4 frag(VS_OUTPUT input) : SV_Target0
{
    float3 base_color = BaseColorTexture.Sample(BaseColorTextureSampler, input.Texcoord).xyz;
    float3 N = CalculateNormal(input.Texcoord, input.Normal, input.Tangent);
    float metallic, roughness;
    GetMetallicRoughness(input.Texcoord, metallic, roughness);
    const float3 dielectric_specular = 0.04;
    
    float3 V = normalize(MeshPerframeBuffer.CameraPosition - input.PositionWorldSpace);
    float3 R = reflect(-V, N);
    
    float3 F0 = lerp(dielectric_specular, base_color, float3(metallic, metallic, metallic));
    
    float3 Lo = 0;
    
    int point_light_num = MeshPerframeBuffer.PointLightNum;
    for (int light_index = 0; light_index < point_light_num; ++light_index)
    {
        // TODO
    }

    {
        float3 directional_color = MeshPerframeBuffer.DirectionalLight.Color;
        float3 L = -normalize(MeshPerframeBuffer.DirectionalLight.Direction);
        float NoL = dot(N, L);
        
        if (NoL > 0.0)
        {
            float4 position_homo = mul(MeshPerframeBuffer.DirectionalLightProjView, float4(input.PositionWorldSpace, 1.0));
            float3 position_ndc = position_homo.xyz / position_homo.w;

            float2 shadow_uv = NDC2UV(position_ndc.xy);
            
            float shadow = GetShadow(shadow_uv, position_ndc.z);
            if (shadow > 0.0)
            {
                Lo += BRDF(N, V, L, F0, base_color, metallic, roughness) * directional_color * NoL * shadow;
            }
        }
    }
    
    Lo += base_color * MeshPerframeBuffer.AmbientLight;
    
    float3 Libl = (float3)0;
    Lo += Libl;
    
    return float4(Lo, 1.0);
}
