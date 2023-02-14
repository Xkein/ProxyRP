#pragma once

#include "core/math/math.h"

class RenderScene;
class RenderCamera;

struct ClusterFrustum
{
    Vector4f PlaneLeft;
    Vector4f PlaneRight;
    Vector4f PlaneTop;
    Vector4f PlaneBottom;
    Vector4f PlaneNear;
    Vector4f PlaneFar;
};

ClusterFrustum CreateClusterFrustumFromMatrix(Matrix4x4 mat,
                                              float     x_left,
                                              float     x_right,
                                              float     y_top,
                                              float     y_bottom,
                                              float     z_near,
                                              float     z_far);

bool TiledFrustumIntersectBox(ClusterFrustum const& f, BoundingBox const& b);

BoundingBox BoundingBoxTransform(BoundingBox const& b, Matrix4x4 const& m);

bool BoxIntersectsWithSphere(BoundingBox const& b, BoundingSphere const& s);

Matrix4x4 CalculateDirectionalLightCamera(RenderScene& scene, RenderCamera& camera);
