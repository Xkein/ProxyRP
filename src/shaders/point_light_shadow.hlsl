
#include "common/constants.hlsl"
#include "common/structures.hlsl"
#include "common/mesh_common.hlsl"

struct MeshPointLightShadowPerframeStorageBufferObject
{
    uint PointLightNum;
    float4 PointLightsPositionAndRadius[MaxPointLightCount];
    float4x4 ProjViewMatrix[6];
};

struct GSSceneIn
{
    float3 Position : Position;
};

struct PSSceneIn
{
    float4 Position : SV_Position;
    int Layer : SV_RenderTargetArrayIndex;
};

#define PointLightPerframeBuffer _PointLightPerframeBuffer[0]
StructuredBuffer<MeshPointLightShadowPerframeStorageBufferObject> _PointLightPerframeBuffer : register(t0);

void vert(float3 position : Position, uint vertex_index : SV_VertexID, uint instance_index : SV_InstanceID, out GSSceneIn output)
{
    float4x4 model_matrix = MeshInstances[instance_index].ModelMatrix;
    float enable_vertex_blending = MeshInstances[instance_index].EnableVertexBlending;
    
    float3 model_position;
    float3 model_normal;
    float3 model_tangent;
    GetMeshBlendingResult(position, (float3) 0, (float3) 0, enable_vertex_blending, vertex_index, instance_index, model_position, model_normal, model_tangent);
    
    float4 position_clip = mul(model_matrix, float4(model_position, 1.0));
    output.Position = position_clip.xyz / position_clip.w;
}

[maxvertexcount(MaxPointLightGeomVertices)]
void geom(triangle GSSceneIn input[3], inout TriangleStream<PSSceneIn> OutputStream)
{
    PSSceneIn output = (PSSceneIn) 0;
    
    int point_light_count = PointLightPerframeBuffer.PointLightNum;
    
    for (int point_light_index = 0; point_light_index < point_light_count; ++point_light_index)
    {
        float3 point_light_position = PointLightPerframeBuffer.PointLightsPositionAndRadius[point_light_index].xyz;
        float point_light_radius = PointLightPerframeBuffer.PointLightsPositionAndRadius[point_light_index].w;
        
        [unroll]
        for (int face = 0; face < 6; ++face)
        {
            for (int vertex_index = 0; vertex_index < 3; ++vertex_index)
            {
                float3 position_world_space = input[vertex_index].Position;
                
                float3 position_view_space = position_world_space - point_light_position;
                
                output.Layer = face + 6 * point_light_index;
                output.Position = mul(PointLightPerframeBuffer.ProjViewMatrix[face], float4(position_view_space, 1.0));
                OutputStream.Append(output);
            }
            OutputStream.RestartStrip();
        }
    }
}


float frag(PSSceneIn input) : SV_Target
{
    float point_light_radius = PointLightPerframeBuffer.PointLightsPositionAndRadius[input.Layer / 6].w;
    float distance = length(input.Position.xyz) / point_light_radius;
    return distance;
}
