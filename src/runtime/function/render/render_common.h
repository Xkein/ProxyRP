#pragma once

#include "core/math/matrix.h"
#include "core/math/vector.h"

// TODO: 64 may not be the best
uint32_t constexpr GMeshPerDrawcallMaxInstanceCount = 64;
uint32_t constexpr GMeshVertexBlendingMaxJointCount = 1024;
uint32_t constexpr GMaxPointLightCount              = 3;

struct SceneDirectionalLight
{
    Vector3f Direction;
    alignas(16) Vector3f Color;
    int _padding;
};

struct ScenePointLight
{
    Vector3f Position;
    float    Radius;
    Vector3f Intensity;
    int      _padding;
};

struct MeshInstance
{
    float EnableVertexBlending;
    alignas(16) Matrix4x4 ModelMatrix;
};

struct MeshPerframeStorageBufferObject
{
    Matrix4x4 ProjViewMatrix;
    Vector3f  CameraPosition;
    alignas(16) Vector3f AmbientLight;
    uint32_t PointLightNum;
    alignas(16) ScenePointLight PointLights[GMaxPointLightCount];
    alignas(16) SceneDirectionalLight DirectionalLight;
    alignas(16) Matrix4x4 DirectionalLightProjView;
};
struct MeshPerdrawcallStorageBufferObject
{
    MeshInstance MeshInstances[GMeshPerDrawcallMaxInstanceCount];
};
struct MeshPerdrawcallVertexBlendingStorageBufferObject
{
    Matrix4x4 JointMatrices[GMeshVertexBlendingMaxJointCount * GMeshPerDrawcallMaxInstanceCount];
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

struct MeshPerMaterialUniformBufferObject
{
    Vector4f baseColorFactor {0.0f, 0.0f, 0.0f, 0.0f};

    float metallicFactor    = 0.0f;
    float roughnessFactor   = 0.0f;
    float normalScale       = 0.0f;
    float occlusionStrength = 0.0f;

    Vector3f emissiveFactor  = {0.0f, 0.0f, 0.0f};
    uint32_t is_blend        = 0;
    uint32_t is_double_sided = 0;
};

struct MeshPointLightShadowPerframeStorageBufferObject
{
    uint32_t PointLightNum;
    uint32_t CurrentFace;
    uint32_t CurrentIndex;
    alignas(16) Vector4f PointLightsPositionAndRadius[GMaxPointLightCount];
    Matrix4x4 ProjViewMatrix[6];
};

struct MeshPointLightShadowPerdrawcallStorageBufferObject
{
    MeshInstance MeshInstances[GMeshPerDrawcallMaxInstanceCount];
};
struct MeshPointLightShadowPerdrawcallVertexBlendingStorageBufferObject
{
    Matrix4x4 JointMatrices[GMeshVertexBlendingMaxJointCount * GMeshPerDrawcallMaxInstanceCount];
};
