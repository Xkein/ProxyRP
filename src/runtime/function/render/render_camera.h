#pragma once

#include "core/math/math.h"

#include <mutex>

class RenderCamera
{
public:

    static constexpr float MIN_FOV = 10.f;
    static constexpr float MAX_FOV = 89.f;

    void SetMainViewMatrix(const Matrix4x4& view_matrix);

    void Move(Vector3f translation);
    /**
     * .
     *
     * \param pitch: in radians
     * \param yaw: in radians
     */
    void Rotate(float pitch, float yaw);
    void Zoom(float offset);
    void LookAt(const Vector3f& position, const Vector3f& target, const Vector3f& up);

    void SetAspect(float aspect)
    {
        Aspect = aspect;

        FovY = RadiansToDegrees(atan(tan(DegreesToRadians(FovX * 0.5f)) / aspect) * 2.f);
    }
    void SetFovX(float fovx)
    {
        FovX = fovx;
    }

    Matrix4x4 GetViewMatrix() const;
    Matrix4x4 GetPerspectiveMatrix() const;

    Vector3f Forward() const
    {
        return (InvRotation * Vector3f::UnitY());
    }
    Vector3f Up() const
    {
        return (InvRotation * Vector3f::UnitZ());
    }
    Vector3f Right() const
    {
        return (InvRotation * Vector3f::UnitX());
    }

	Vector3f    Position {Vector3f::Zero()};
    Quaternionf Rotation {Quaternionf::Identity()};
    Quaternionf InvRotation {Quaternionf::Identity()};
    float       ZNear {1000.f};
    float       ZFar {0.1f};
    Vector3f    UpAxis {Vector3f::UnitZ()};
	
    Matrix4x4 ViewMatrix {Matrix4x4::Identity()};


private:
    float Aspect {0.f};
    float FovX {89.f};
    float FovY {0.f};

    std::mutex ViewMatrixMutex;
};
