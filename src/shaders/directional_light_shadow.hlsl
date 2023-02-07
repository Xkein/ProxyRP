
#include "common/constants.hlsl"
#include "common/structures.hlsl"
#include "common/mesh_common.hlsl"

StructuredBuffer<float4x4> LightProjView : register(t0);

void vert(float3 position : POSITION, uint vertex_index : SV_VertexID, uint instance_index : SV_InstanceID, out float4 position_mvp : SV_Position)
{
    float4x4 model_matrix = MeshInstances[instance_index].ModelMatrix;
    float enable_vertex_blending = MeshInstances[instance_index].EnableVertexBlending;
    
    float3 model_position;
    float3 model_normal;
    float3 model_tangent;
    GetMeshBlendingResult(position, (float3)0, (float3)0, enable_vertex_blending, vertex_index, instance_index, model_position, model_normal, model_tangent);
    
    float3 position_world_space;
    GetPosition(model_matrix, LightProjView[0], model_position, position_world_space, position_mvp);
}

[earlydepthstencil]
float4 frag(float4 position : SV_Position) : SV_Target
{
    return position.z / position.w;
}
