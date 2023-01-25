#include "math_serializer.h"

template<typename Scalar_, int Rows_, int Cols_, int Options_, int MaxRows_, int MaxCols_>
inline bool ReadEigenMatrix(const json&                                                         context,
                            Eigen::Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>& instance)
{
    assert(context.is_array());
    for (size_t i = 0; i < context.size(); i++)
    {
        instance.data()[i] = context[i].get<float>();
    }
    return true;
}
template<typename Scalar_, int Rows_, int Cols_, int Options_, int MaxRows_, int MaxCols_>
inline bool WriteEigenMatrix(const Eigen::Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>& instance,
                             json&                                                                     context)
{
    for (size_t i = 0; i < instance.size(); i++)
    {
        context.emplace_back(instance.data()[i]);
    }
    return true;
}



// serializer for vector

template<>
bool Serializer::Read(const json& context, Vector2f& instance)
{
    return ReadEigenMatrix(context, instance);
}
template<>
bool Serializer::Write(const Vector2f& instance, json& context)
{
    return WriteEigenMatrix(instance, context);
}

template<>
bool Serializer::Read(const json& context, Vector3f& instance)
{
    return ReadEigenMatrix(context, instance);
}
template<>
bool Serializer::Write(const Vector3f& instance, json& context)
{
    return WriteEigenMatrix(instance, context);
}

template<>
bool Serializer::Read(const json& context, Vector4f& instance)
{
    return ReadEigenMatrix(context, instance);
}
template<>
bool Serializer::Write(const Vector4f& instance, json& context)
{
    return WriteEigenMatrix(instance, context);
}

// serializer for matrix

template<>
bool Serializer::Read(const json& context, Matrix3x3& instance)
{
    return ReadEigenMatrix(context, instance);
}
template<>
bool Serializer::Write(const Matrix3x3& instance, json& context)
{
    return WriteEigenMatrix(instance, context);
}

template<>
bool Serializer::Read(const json& context, Matrix4x4& instance)
{
    return ReadEigenMatrix(context, instance);
}
template<>
bool Serializer::Write(const Matrix4x4& instance, json& context)
{
    return WriteEigenMatrix(instance, context);
}

// serializer for quaternion

template<>
bool Serializer::Read(const json& context, Quaternionf& instance)
{
    return ReadEigenMatrix(context, instance.coeffs());
}
template<>
bool Serializer::Write(const Quaternionf& instance, json& context)
{
    return WriteEigenMatrix(instance.coeffs(), context);
}

// serializer for axis aligned bounding box

template<>
bool Serializer::Read(const json& context, AxisAlignedBox& instance)
{
    ASSERT(context.is_object());
    bool ret = true;

    if (!context["Min"].is_null())
    {
        ret &= ReadEigenMatrix(context["Min"], instance.min());
    }
    if (!context["Max"].is_null())
    {
        ret &= ReadEigenMatrix(context["Max"], instance.max());
    }

    return ret;
}
template<>
bool Serializer::Write(const AxisAlignedBox& instance, json& context)
{
    bool ret = true;

    json min_json;
    ret &= WriteEigenMatrix(instance.min(), min_json);
    context["Min"] = min_json;

    json max_json;
    ret &= WriteEigenMatrix(instance.max(), max_json);
    context["Max"] = max_json;

    return ret;
}
