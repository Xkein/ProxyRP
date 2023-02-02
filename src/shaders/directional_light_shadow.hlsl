
#include "common/constants.hlsl"
#include "common/structures.hlsl"

StructuredBuffer<float4x4> LightProjView : register(t0);
StructuredBuffer<MeshInstance> MeshInstances : register(t1);
StructuredBuffer<float4x4> JointMatrices : register(t2);

StructuredBuffer<MeshVertexJointBinding> JointBindings : register(t0, space1);

float4 vert(float3 position : POSITION, uint vertex_index: SV_VertexID, uint instance_index : SV_InstanceID) : SV_Position
{
    float4x4 model_matrix = MeshInstances[instance_index].ModelMatrix;
    float enable_vertex_blending = MeshInstances[instance_index].EnableVertexBlending;
    
    float3 model_position;
    if (enable_vertex_blending > 0.0)
    {
        // TODO
    }
    else
    {
        model_position = position;
    }
    
    float3 position_world_space = mul(model_matrix, float4(model_position, 1.0)).xyz;
    
    return mul(LightProjView[0], float4(position_world_space, 1.0));
}

[earlydepthstencil]
float4 frag(float4 position : SV_Position) : SV_Target
{
    return position.z;
}
