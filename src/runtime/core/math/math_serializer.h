#pragma once

#include "core/serializer/serializer_base.h"
#include "math.h"

// serializer for vector

template<>
bool Serializer::Read(const json& context, Vector2f& instance);
template<>
bool Serializer::Write(const Vector2f& instance, json& context);

template<>
bool Serializer::Read(const json& context, Vector3f& instance);
template<>
bool Serializer::Write(const Vector3f& instance, json& context);

template<>
bool Serializer::Read(const json& context, Vector4f& instance);
template<>
bool Serializer::Write(const Vector4f& instance, json& context);

// serializer for matrix

template<>
bool Serializer::Read(const json& context, Matrix3x3& instance);
template<>
bool Serializer::Write(const Matrix3x3& instance, json& context);

template<>
bool Serializer::Read(const json& context, Matrix4x4& instance);
template<>
bool Serializer::Write(const Matrix4x4& instance, json& context);

// serializer for quaternion

template<>
bool Serializer::Read(const json& context, Quaternionf& instance);
template<>
bool Serializer::Write(const Quaternionf& instance, json& context);

// serializer for axis aligned bounding box

template<>
bool Serializer::Read(const json& context, AxisAlignedBox& instance);
template<>
bool Serializer::Write(const AxisAlignedBox& instance, json& context);

