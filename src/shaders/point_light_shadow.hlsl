
#include "common/constants.hlsl"
#include "common/structures.hlsl"
#include "common/mesh_common.hlsl"

struct MeshPointLightShadowPerframeStorageBufferObject
{
    uint PointLightNum;
    float4 PointLightsPositionAndRadius[MaxPointLightCount];
};

#define PointLightPerframeBuffer _PointLightPerframeBuffer[0]
StructuredBuffer<MeshPointLightShadowPerframeStorageBufferObject> _PointLightPerframeBuffer : register(t0);

float3 vert(float3 position : POSITION, uint vertex_index : SV_VertexID, uint instance_index : SV_InstanceID) : Position
{
    float4x4 model_matrix = MeshInstances[instance_index].ModelMatrix;
    float enable_vertex_blending = MeshInstances[instance_index].EnableVertexBlending;
    
    float3 model_position;
    float3 model_normal;
    float3 model_tangent;
    GetMeshBlendingResult(position, (float3) 0, (float3) 0, enable_vertex_blending, vertex_index, instance_index, model_position, model_normal, model_tangent);
    
    float4 position_homo = mul(model_matrix, float4(model_position, 1.0));
    return position_homo.xyz / position_homo.w;
}

struct GSSceneIn
{
    
};

struct PSSceneIn
{
    float4 Position : SV_Position;
    int Layer : SV_RenderTargetArrayIndex;
};


float4x4 LookAt(float3 eye_position, float3 target_position, float3 up_dir)
{
    float3 f = normalize(target_position - eye_position);
    float3 s = normalize(cross(f, up_dir));
    float3 u = cross(s, f);
    
    float4x4 view_mat;
    view_mat[0] = float4(s, -dot(s, eye_position));
    view_mat[1] = float4(u, -dot(u, eye_position));
    view_mat[2] = float4(-f, dot(f, eye_position));
    view_mat[3] = float4(0, 0, 0, 1);
    return view_mat;
}

float4x4 Perspective(float fovy, float aspect, float znear, float zfar)
{
    float tan_half_fovy = tan(fovy / 2);

    float4x4 persp_mat;
    persp_mat[0, 0] = 1 / (aspect * tan_half_fovy);
    persp_mat[1, 1] = 1 / tan_half_fovy;
    persp_mat[2, 2] = zfar / (znear - zfar);
    persp_mat[3, 2] = -1;
    persp_mat[2, 3] = -(zfar * znear) / (zfar - znear);
    return persp_mat;
}

[maxvertexcount(MaxPointLightGeomVertices)]
void geom(triangle float3 input[3], inout TriangleStream<PSSceneIn> OutputStream)
{
    PSSceneIn output = (PSSceneIn) 0;
    const float4x4 proj_matrix = Perspective(radians(90), 1, 0.001, 25);
    const float4x4 proj_view_matrix[6] =
    {
        proj_matrix * LookAt(0, float3(1, 0, 0), float3(0, 0, 1)),
        proj_matrix * LookAt(0, float3(-1, 0, 0), float3(0, 0, 1)),
        proj_matrix * LookAt(0, float3(0, 1, 0), float3(0, 0, 1)),
        proj_matrix * LookAt(0, float3(0, -1, 0), float3(0, 0, 1)),
        proj_matrix * LookAt(0, float3(0, 0, 1), float3(0, 0, 1)),
        proj_matrix * LookAt(0, float3(0, 0, -1), float3(0, 0, -1)),
    };
    
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
                float3 position_world_space = input[vertex_index];
                
                float3 position_view_space = position_world_space - point_light_position;
                
                output.Layer = face + 6 * point_light_index;
                output.Position = mul(proj_view_matrix[face], float4(position_view_space, 1.0));
                
                OutputStream.Append(output);
            }
            OutputStream.RestartStrip();
        }
    }
}

[earlydepthstencil]
float frag(PSSceneIn input) : SV_Target
{
    float point_light_radius = PointLightPerframeBuffer.PointLightsPositionAndRadius[input.Layer / 6].w;
    float distance = length(input.Position.xyz) / point_light_radius;
    return distance;
}
