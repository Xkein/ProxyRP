#pragma once

#include "constants.hlsl"

struct MeshInstance
{
    float EnableVertexBlending;
    
    float4x4 ModelMatrix;
};

struct MeshVertexJointBinding
{
    float4 Indices;
    float4 Weights;
};

struct SceneDirectionalLight
{
    float3 Direction;
    float3 Color;
};

struct ScenePointLight
{
    float3 Position;
    float  Radius;
    float3 Intensity;
};

struct MeshPerframeStorageBufferObject
{
    float4x4 ProjViewMatrix;
    float3   CameraPosition;
    float3   AmbientLight;
    uint     PointLightNum;
    ScenePointLight       PointLights[MaxPointLightCount];
    SceneDirectionalLight DirectionalLight;
    float4x4              DirectionalLightProjView;
};
