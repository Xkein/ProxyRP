#pragma once

#include "core/math/matrix.h"
#include "core/math/vector.h"

// TODO: 64 may not be the best
uint32_t constexpr GMeshPerDrawcallMaxInstanceCount = 64;
uint32_t constexpr GMeshVertexBlendingMaxJointCount = 1024;
uint32_t constexpr GMaxPointLightCount              = 15;

// TODO: 64 may not be the best
uint32_t constexpr MeshPerDrawcallMaxInstanceCount = 64;
uint32_t constexpr MeshVertexBlendingMaxJointCount = 1024;
uint32_t constexpr MaxPointLightCount              = 15;

struct SceneDirectionalLight
{
    Vector3f Direction;
    alignas(16) Vector3f Color;
};

struct ScenePointLight
{
    Vector3f Position;
    float    Radius;
    alignas(16) Vector3f Intensity;
};

struct MeshPerframeStorageBufferObject
{
    Matrix4x4 ProjViewMatrix;
    Vector3f  CameraPosition;
    alignas(16) Vector3f AmbientLight;
    alignas(16) uint32_t PointLightNum;
    alignas(16) ScenePointLight scenePointLights[GMaxPointLightCount];
    alignas(16) SceneDirectionalLight sceneDirectionalLight;
    alignas(16) Matrix4x4 DirectionalLightProjView;
};

struct MeshInstance
{
    float EnableVertexBlending;
    alignas(16) Matrix4x4 ModelMatrix;
};


struct MeshDirectionalLightShadowPerframeStorageBufferObject
{
    Matrix4x4 LightProjView;
};

struct MeshDirectionalLightShadowPerdrawcallStorageBufferObject
{
    MeshInstance MeshInstances[GMeshPerDrawcallMaxInstanceCount];
};

struct MeshDirectionalLightShadowPerdrawcallVertexBlendingStorageBufferObject
{
    Matrix4x4 JointMatrices[GMeshVertexBlendingMaxJointCount * GMeshPerDrawcallMaxInstanceCount];
};

