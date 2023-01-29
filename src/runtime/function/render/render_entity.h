#pragma once

#include "core/math/axis_aligned.h"
#include "core/math/matrix.h"

#include <stdint.h>
#include <vector>

class RenderEntity
{
public:
    uint32_t  InstanceId {0};
    Matrix4x4 ModelMatrix {Matrix4x4::Identity()};

    // mesh
    uint32_t               MeshAssetId {0};
    bool                   EnableVertexBlending {false};
    std::vector<Matrix4x4> JointMatrices;
    AxisAlignedBox         BoundingBox;

    // material
    uint32_t               MaterialAssetId {0};
};
