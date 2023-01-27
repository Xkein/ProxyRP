#pragma once

#include "angle.h"
#include "axis_aligned.h"
#include "matrix.h"
#include "vector.h"
#include "quaternion.h"
#include "transform.h"

Matrix4x4 LookAt(const Vector3f& eye_position, const Vector3f& target_position, const Vector3f& up_dir);

/**
 * .
 *
 * \param fovy: radian of angle
 * \param aspect
 * \param znear
 * \param zfar
 * \return
 */
Matrix4x4 Perspective(float fovy, float aspect, float znear, float zfar);

Matrix4x4 Orthographic(float left, float right, float bottom, float top, float znear, float zfar);

template<typename T>
T RoundUp(T value, T alignment)
{
    static_assert(std::is_integral_v<T>, "RoundUp is for integer only.");
    T temp = value + alignment - static_cast<T>(1);
    return temp - temp % alignment;
}

