#include "render_camera.h"
#include "core/math/math.h"

void RenderCamera::SetMainViewMatrix(const Matrix4x4& view_matrix)
{
    ViewMatrix = view_matrix;

    Vector3f s = Vector3f(view_matrix(0, 0), view_matrix(0, 1), view_matrix(0, 2));
    Vector3f u = Vector3f(view_matrix(1, 0), view_matrix(1, 1), view_matrix(1, 2));
    Vector3f f = Vector3f(-view_matrix(2, 0), -view_matrix(2, 1), -view_matrix(2, 2));
    Position   = s * (-view_matrix(0, 3)) + u * (-view_matrix(1, 3)) + f * view_matrix(2, 3);
}

void RenderCamera::Move(Vector3f translation)
{
    Position += translation;
}

void RenderCamera::Rotate(float pitch, float yaw)
{
    // limit pitch
    float dot = UpAxis.dot(Forward());
    if ((dot < -0.99f && pitch > 0.0f) || // angle nearing 180 degrees
        (dot > 0.99f && pitch < 0.0f))    // angle nearing 0 degrees
        pitch = 0.0f;

    // pitch is relative to current sideways rotation
    // yaw happens independently
    // this prevents roll
    Rotation    = AngleAxisf(pitch, Vector3f::UnitX()) * Rotation * AngleAxisf(yaw, Vector3f::UnitZ());
    InvRotation = Rotation.conjugate();
}


void RenderCamera::Zoom(float offset)
{
    // > 0 = zoom in (decrease FOV by <offset> angles)
    FovX = std::clamp(FovX - offset, MIN_FOV, MAX_FOV);
}

void RenderCamera::LookAt(const Vector3f& position, const Vector3f& target, const Vector3f& up)
{
    Position = position;

    // model rotation
    // maps vectors to camera space (x, y, z)
    Vector3f forward = (target - position).normalized();
    Rotation         = Quaternionf::FromTwoVectors(forward, Vector3f::UnitY());

    // correct the up vector
    // the cross product of non-orthogonal vectors is not normalized
    Vector3f right = forward.cross(up.normalized()).normalized();
    Vector3f orth_up = right.cross(forward);

    Quaternionf up_rotation = Quaternionf::FromTwoVectors(Rotation * orth_up, Vector3f::UnitZ());

    // inverse of the model rotation
    // maps camera space vectors to model vectors
    InvRotation = Rotation.conjugate();
}

Matrix4x4 RenderCamera::GetViewMatrix() const
{
    return ViewMatrix;
}

Matrix4x4 RenderCamera::GetPerspectiveMatrix() const
{
    Matrix4x4 fix_mat {{1, 0, 0, 0}, {0, -1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    Matrix4x4 proj_mat = fix_mat * Perspective(DegreesToRadians(FovY), Aspect, ZNear, ZFar);
    return proj_mat;
}
