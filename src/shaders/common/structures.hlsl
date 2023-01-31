#pragma once

struct MeshInstance
{
    float EnableVertexBlending;
    float _padding_enable_vertex_blending_1;
    float _padding_enable_vertex_blending_2;
    float _padding_enable_vertex_blending_3;
    float4x4 ModelMatrix;
};

struct MeshVertexJointBinding
{
    float4 Indices;
    float4 Weights;
};
