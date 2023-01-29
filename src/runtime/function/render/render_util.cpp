#include "render_util.h"
#include "render_scene.h"
#include "render_camera.h"
#include "core/macro.h"

ClusterFrustum CreateClusterFrustumFromMatrix(Matrix4x4 mat,
                                              float     x_left,
                                              float     x_right,
                                              float     y_top,
                                              float     y_bottom,
                                              float     z_near,
                                              float     z_far)
{
    ClusterFrustum f;

    // the following is in the vulkan space
    // note that the Y axis is flipped in Vulkan
    ASSERT(y_top < y_bottom);

    ASSERT(x_left < x_right);
    ASSERT(z_near < z_far);

    // calculate the tiled frustum
    // [Fast Extraction of Viewing Frustum Planes from the WorldView - Projection
    // Matrix](http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf)

    Matrix4x4 mat_column = mat;

    // [vec.xyz 1][mat.col0] / [vec.xyz 1][mat.col3] > x_right
    // [vec.xyz 1][mat.col0 - mat.col3*x_right] > 0
    f.PlaneRight = Vector4f(mat_column.row(0)) - (Vector4f(mat_column.row(3)) * x_right);
    // normalize
    f.PlaneRight *= (1.0 / Vector3f(f.PlaneRight.head<3>()).norm());

    // for example, we try to calculate the "plane_left" of the tile frustum
    // note that we use the row vector to be consistent with the DirectXMath
    // [vec.xyz 1][mat.col0] / [vec.xyz 1][mat.col3] < x_left
    //
    // evidently, the value "[vec.xyz 1][mat.col3]" is expected to be greater than
    // 0 (w of clip space) and we multiply both sides by "[vec.xyz 1][mat.col3]",
    // the inequality symbol remains the same [vec.xyz 1][mat.col0] < [vec.xyz
    // 1][mat.col3]*x_left
    //
    // since "x_left" is a scalar, the "scalar multiplication" is applied
    // [vec.xyz 1][mat.col0] < [vec.xyz 1][mat.col3*x_left]
    // [vec.xyz 1][mat.col0 - mat.col3*x_left] < 0
    //
    // we follow the "DirectX::BoundingFrustum::Intersects", the normal of the
    // plane is pointing ourward [vec.xyz 1][mat.col3*x_left - mat.col0] > 0
    //
    // the plane can be defined as [x y z 1][A B C D] = 0 and the [A B C D] is
    // exactly [mat.col0 - mat.col3*x_left] and we need to normalize the normal[A
    // B C] of the plane let [A B C D] = [mat.col3*x_left - mat.col0] [A B C D] /=
    // length([A B C].xyz)
    f.PlaneLeft = (Vector4f(mat_column.row(3)) * x_left) - Vector4f(mat_column.row(0));
    // normalize
    f.PlaneLeft *= (1.0 / Vector3f(f.PlaneLeft.head<3>()).norm());

    // [vec.xyz 1][mat.col1] / [vec.xyz 1][mat.col3] < y_top
    // [vec.xyz 1][mat.col3*y_top - mat.col1] > 0
    f.PlaneTop = (Vector4f(mat_column.row(3)) * y_top) - Vector4f(mat_column.row(1));
    // normalize
    f.PlaneTop *= (1.0 / Vector3f(f.PlaneTop.head<3>()).norm());

    // [vec.xyz 1][mat.col1] / [vec.xyz 1][mat.col3] > y_bottom
    // [vec.xyz 1][mat.col1 - mat.col3*y_bottom] > 0
    f.PlaneBottom = Vector4f(mat_column.row(1)) - (Vector4f(mat_column.row(3)) * y_bottom);
    // normalize
    f.PlaneBottom *= (1.0 / Vector3f(f.PlaneBottom.head<3>()).norm());

    // [vec.xyz 1][mat.col2] / [vec.xyz 1][mat.col3] < z_near
    // [vec.xyz 1][mat.col3*z_near - mat.col2] > 0
    f.PlaneNear = (Vector4f(mat_column.row(3)) * z_near) - Vector4f(mat_column.row(2));
    f.PlaneNear *= (1.0 / Vector3f(f.PlaneNear.head<3>()).norm());

    // [vec.xyz 1][mat.col2] / [vec.xyz 1][mat.col3] > z_far
    // [vec.xyz 1][mat.col2 - mat.col3*z_far] > 0
    f.PlaneFar = Vector4f(mat_column.row(2)) - (Vector4f(mat_column.row(3)) * z_far);
    f.PlaneFar *= (1.0 / Vector3f(f.PlaneFar.head<3>()).norm());

    return f;
}

bool TiledFrustumIntersectBox(ClusterFrustum const& f, BoundingBox const& b)
{
    // we follow the "DirectX::BoundingFrustum::Intersects"

    // Center of the box.
    Vector4f box_center(b.center().homogeneous());

    // Distance from the center to each side.
    // half extent //more exactly
    Vector3f box_extents(b.sizes() / 2.f);

    // plane_right
    {
        float signed_distance_from_plane_right = f.PlaneRight.dot(box_center);
        float radius_project_plane_right       = Vector3f(f.PlaneRight.head<3>()).cwiseAbs().dot(box_extents);

        bool intersecting_or_inside_right = signed_distance_from_plane_right < radius_project_plane_right;
        if (!intersecting_or_inside_right)
        {
            return false;
        }
    }

    // plane_left
    {
        float signed_distance_from_plane_left = f.PlaneLeft.dot(box_center);
        float radius_project_plane_left       = Vector3f(f.PlaneLeft.head<3>()).cwiseAbs().dot(box_extents);

        bool intersecting_or_inside_left = signed_distance_from_plane_left < radius_project_plane_left;
        if (!intersecting_or_inside_left)
        {
            return false;
        }
    }

    // plane_top
    {
        float signed_distance_from_plane_top = f.PlaneTop.dot(box_center);
        float radius_project_plane_top       = Vector3f(f.PlaneTop.head<3>()).cwiseAbs().dot(box_extents);

        bool intersecting_or_inside_top = signed_distance_from_plane_top < radius_project_plane_top;
        if (!intersecting_or_inside_top)
        {
            return false;
        }
    }

    // plane_bottom
    {
        float signed_distance_from_plane_bottom = f.PlaneBottom.dot(box_center);
        float radius_project_plane_bottom       = Vector3f(f.PlaneBottom.head<3>()).cwiseAbs()
                .dot(box_extents);

        bool intersecting_or_inside_bottom = signed_distance_from_plane_bottom < radius_project_plane_bottom;
        if (!intersecting_or_inside_bottom)
        {
            return false;
        }
    }

    // plane_near
    {
        float signed_distance_from_plane_near = f.PlaneNear.dot(box_center);
        float radius_project_plane_near       = Vector3f(f.PlaneNear.head<3>()).cwiseAbs().dot(box_extents);

        bool intersecting_or_inside_near = signed_distance_from_plane_near < radius_project_plane_near;
        if (!intersecting_or_inside_near)
        {
            return false;
        }
    }

    // plane_far
    {
        float signed_distance_from_plane_far = f.PlaneFar.dot(box_center);
        float radius_project_plane_far       = Vector3f(f.PlaneFar.head<3>()).cwiseAbs().dot(box_extents);

        bool intersecting_or_inside_far = signed_distance_from_plane_far < radius_project_plane_far;
        if (!intersecting_or_inside_far)
        {
            return false;
        }
    }

    return true;
}

BoundingBox BoundingBoxTransform(BoundingBox const& b, Matrix4x4 const& m)
{
    return b.transformed(Eigen::Affine3f(m));
}

bool BoxIntersectsWithSphere(BoundingBox const& b, BoundingSphere const& s)
{
    for (size_t i = 0; i < 3; ++i)
    {
        if (s.Center[i] < b.min()[i])
        {
            if ((b.min()[i] - s.Center[i]) > s.Radius)
            {
                return false;
            }
        }
        else if (s.Center[i] > b.max()[i])
        {
            if ((s.Center[i] - b.max()[i]) > s.Radius)
            {
                return false;
            }
        }
    }

    return true;
}

Matrix4x4 CalculateDirectionalLightCamera(RenderScene& scene, RenderCamera& camera)
{
    Matrix4x4 proj_view_matrix;
    {
        Matrix4x4 view_matrix = camera.GetViewMatrix();
        Matrix4x4 proj_matrix = camera.GetPerspectiveMatrix();
        proj_view_matrix      = proj_matrix * view_matrix;
    }

    
    BoundingBox frustum_bounding_box;
    // CascadedShadowMaps11 / CreateFrustumPointsFromCascadeInterval
    {
        Vector3f const g_frustum_points_ndc_space[8] = {Vector3f(-1.0f, -1.0f, 1.0f),
                                                        Vector3f(1.0f, -1.0f, 1.0f),
                                                        Vector3f(1.0f, 1.0f, 1.0f),
                                                        Vector3f(-1.0f, 1.0f, 1.0f),
                                                        Vector3f(-1.0f, -1.0f, 0.0f),
                                                        Vector3f(1.0f, -1.0f, 0.0f),
                                                        Vector3f(1.0f, 1.0f, 0.0f),
                                                        Vector3f(-1.0f, 1.0f, 0.0f)};

        Matrix4x4 inverse_proj_view_matrix = proj_view_matrix.inverse();

        frustum_bounding_box.min() = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
        frustum_bounding_box.max() = Vector3f(FLT_MIN, FLT_MIN, FLT_MIN);

        size_t const CORNER_COUNT = 8;
        for (size_t i = 0; i < CORNER_COUNT; ++i)
        {
            Vector4f frustum_point_with_w = inverse_proj_view_matrix * Vector4f(g_frustum_points_ndc_space[i].homogeneous());
            Vector3f frustum_point        = Vector3f(frustum_point_with_w.head<3>()) / frustum_point_with_w.w();

            frustum_bounding_box.extend(frustum_point);
        }
    }

    BoundingBox scene_bounding_box;
    {
        scene_bounding_box.min() = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
        scene_bounding_box.max() = Vector3f(FLT_MIN, FLT_MIN, FLT_MIN);

        for (const RenderEntity& entity : scene.RenderEntities)
        {
            BoundingBox mesh_asset_bounding_box = entity.BoundingBox;

            BoundingBox mesh_bounding_box_world = BoundingBoxTransform(mesh_asset_bounding_box, entity.ModelMatrix);
            scene_bounding_box = scene_bounding_box.merged(mesh_bounding_box_world);
        }
    }
    
    // CascadedShadowMaps11 / ComputeNearAndFar
    Matrix4x4 light_view;
    Matrix4x4 light_proj;
    {
        Vector3f box_center = frustum_bounding_box.center();
        Vector3f box_extents = frustum_bounding_box.sizes() / 2.f;

        Vector3f eye    = box_center + scene.Light.Directional.Direction * box_extents.norm();
        Vector3f center = box_center;
        light_view      = LookAt(eye, center, Vector3f(0.0, 0.0, 1.0));

        BoundingBox frustum_bounding_box_light_view = BoundingBoxTransform(frustum_bounding_box, light_view);
        BoundingBox scene_bounding_box_light_view   = BoundingBoxTransform(scene_bounding_box, light_view);
        light_proj =
            Orthographic(std::max(frustum_bounding_box_light_view.min().x(), scene_bounding_box_light_view.min().x()),
                         std::min(frustum_bounding_box_light_view.max().x(), scene_bounding_box_light_view.max().x()),
                         std::max(frustum_bounding_box_light_view.min().y(), scene_bounding_box_light_view.min().y()),
                         std::min(frustum_bounding_box_light_view.max().y(), scene_bounding_box_light_view.max().y()),
                         -scene_bounding_box_light_view.max().z(),
                         // the objects which are nearer than the frustum bounding box may caster shadow as well
                         -std::max(frustum_bounding_box_light_view.min().z(), scene_bounding_box_light_view.min().z()));
    }

    Matrix4x4 light_proj_view = (light_proj * light_view);
    return light_proj_view;
}
