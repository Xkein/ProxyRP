#pragma once

#include "angle.h"
#include "axis_aligned.h"
#include "matrix.h"
#include "vector.h"
#include "quaternion.h"
#include "transform.h"


Matrix4x4
MakeViewMatrix(const Vector3f& position, const Quaternionf& orientation, const Matrix4x4* reflect_matrix = nullptr);

Matrix4x4
LookAt(const Vector3f& eye_position, const Vector3f& target_position, const Vector3f& up_dir);

Matrix4x4 Perspective(float fovy, float aspect, float znear, float zfar);

Matrix4x4
Orthographic(float left, float right, float bottom, float top, float znear, float zfar);
