
#include "math.h"

Matrix4x4 LookAt(const Vector3f& eye_position, const Vector3f& target_position, const Vector3f& up_dir)
{
    const Vector3f up = up_dir.normalized();

    Vector3f f = (target_position - eye_position).normalized();
    Vector3f s = f.cross(up).normalized();
    Vector3f u = s.cross(f);

    Matrix4x4 view_mat;
    view_mat << s, -s.dot(eye_position),
                u, -u.dot(eye_position),
                -f, f.dot(eye_position),
                0, 0, 0, 1;
    return view_mat;
}

Matrix4x4 Perspective(float fovy, float aspect, float znear, float zfar)
{
    float tan_half_fovy = std::tan(fovy / static_cast<float>(2));

    Matrix4x4 persp_mat = Matrix4x4::Zero();
    persp_mat(0, 0)     = static_cast<float>(1) / (aspect * tan_half_fovy);
    persp_mat(1, 1)     = static_cast<float>(1) / tan_half_fovy;
    persp_mat(2, 2)     = zfar / (znear - zfar);
    persp_mat(3, 2)     = -static_cast<float>(1);
    persp_mat(2, 3)     = -(zfar * znear) / (zfar - znear);
    return persp_mat;
}

Matrix4x4 Orthographic(float left, float right, float bottom, float top, float znear, float zfar)
{
    Matrix4x4 ortho_mat = Matrix4x4::Identity();
    ortho_mat(0, 0)     = static_cast<float>(2) / (right - left);
    ortho_mat(1, 1)     = static_cast<float>(2) / (top - bottom);
    ortho_mat(2, 2)     = -static_cast<float>(1) / (zfar - znear);
    ortho_mat(0, 3)     = -(right + left) / (right - left);
    ortho_mat(1, 3)     = -(top + bottom) / (top - bottom);
    ortho_mat(2, 3)     = -znear / (zfar - znear);
    return ortho_mat;
}
