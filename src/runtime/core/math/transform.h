#pragma once

#include "core/reflection/reflection.h"
#include "core/math/quaternion.h"
#include "core/math/vector.h"
#include "core/math/matrix.h"

#include <Eigen/Geometry>

// using Transformf = Eigen::Transform<float, 3, Eigen::Affine>;

CLASS(Transformf)
{
    REFLECTION_CLASS_BODY(Transformf);

public:
    Transformf() = default;
    Transformf(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale) :
        Position(position), Scale(scale), Rotation(rotation)
    {}

    Matrix4x4 GetMatrix() const {
        Eigen::Affine3f transform =
            Eigen::Translation3f(Position) * Eigen::AngleAxisf(Rotation) * Eigen::Scaling(Scale);
        return transform.matrix();
    }

    PROPERTY()
    Vector3f Position {Vector3f::Zero()};
    PROPERTY()
    Vector3f Scale {Vector3f::Identity()};
    PROPERTY()
    Quaternionf Rotation {Quaternionf::Identity()};
};
