
#include "common/constants.hlsl"
#include "common/math.hlsl"

struct SceneDirectionalLight
{
    float3 Direction;
    float3 Color;
};

struct ScenePointLight
{
    float3 Position;
    float Radius;
    float3 Intensity;
};

struct SkyboxPerframeStorageBufferObject
{
    float4x4 ProjViewMatrix;
    float3 CameraPosition;
    float3 AmbientLight;
    uint PointLightNum;
    ScenePointLight PointLights[MaxPointLightCount];
    SceneDirectionalLight DirectionalLight;
    float4x4 DirectionalLightProjView;
};

#define SkyboxPerFrame _SkyboxPerFrame[0]
StructuredBuffer<SkyboxPerframeStorageBufferObject> _SkyboxPerFrame : register(t0);

TextureCube  SkyboxTextureCube : register(t1);
SamplerState SkyboxTextureCubeSampler : register(s1);

float4 vert(uint vertex_index : SV_VertexID, out float3 UVW : TEXCOORD0) : SV_Position
{
    const float3 cube_corner_vertex_offsets[8] =
    {
        float3(1.0, 1.0, 1.0),
        float3(1.0, 1.0, -1.0),
        float3(1.0, -1.0, -1.0),
        float3(1.0, -1.0, 1.0),
        float3(-1.0, 1.0, 1.0),
        float3(-1.0, 1.0, -1.0),
        float3(-1.0, -1.0, -1.0),
        float3(-1.0, -1.0, 1.0)
    };
    // x+, y+, x-, y-, z+, z-
    const int cube_triangle_index[36] = { 0, 1, 2, 2, 3, 0, 4, 5, 1, 1, 0, 4, 7, 6, 5, 5, 4, 7, 3, 2, 6, 6, 7, 3, 4, 0, 3, 3, 7, 4, 1, 5, 6, 6, 2, 1 };
    
    float3 position_world_space = SkyboxPerFrame.CameraPosition + cube_corner_vertex_offsets[cube_triangle_index[vertex_index]];
    float4 position_clip = mul(SkyboxPerFrame.ProjViewMatrix, float4(position_world_space, 1.0));
    
    position_clip.z = position_clip.w * 0.99999;
    
    UVW = normalize(position_world_space - SkyboxPerFrame.CameraPosition);
    
    return position_clip;
}

void frag(float3 UVW : TEXCOORD0, out float4 out_color : SV_Target)
{
    float3 color = SampleCubeMapWithAdjustedUVW(SkyboxTextureCube, SkyboxTextureCubeSampler, UVW, 0).rgb;
    //out_color = float4(color, 1);
    out_color = float4(pow(color, 2.2), 1.0);
}
