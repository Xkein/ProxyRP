#include "serializer.h"

template<typename T>
inline bool ReadPrimitive(const json& context, T& instance)
{
    instance = context.get<T>();
    return true;
}
template<typename T>
inline bool WritePrimitive(const T& instance, json& context)
{
    context = instance;
    return true;
}

template<>
bool Serializer::Read(const json& context, bool& instance)
{
    return ReadPrimitive(context, instance);
}
template<>
bool Serializer::Write(const bool& instance, json& context)
{
    return WritePrimitive(instance, context);
}

template<>
bool Serializer::Read(const json& context, char& instance)
{
    return ReadPrimitive(context, instance);
}
template<>
bool Serializer::Write(const char& instance, json& context)
{
    return WritePrimitive(instance, context);
}

template<>
bool Serializer::Read(const json& context, unsigned char& instance)
{
    return ReadPrimitive(context, instance);
}
template<>
bool Serializer::Write(const unsigned char& instance, json& context)
{
    return WritePrimitive(instance, context);
}

template<>
bool Serializer::Read(const json& context, short& instance)
{
    return ReadPrimitive(context, instance);
}
template<>
bool Serializer::Write(const short& instance, json& context)
{
    return WritePrimitive(instance, context);
}

template<>
bool Serializer::Read(const json& context, unsigned short& instance)
{
    return ReadPrimitive(context, instance);
}
template<>
bool Serializer::Write(const unsigned short& instance, json& context)
{
    return WritePrimitive(instance, context);
}

template<>
bool Serializer::Read(const json& context, int& instance)
{
    return ReadPrimitive(context, instance);
}
template<>
bool Serializer::Write(const int& instance, json& context)
{
    return WritePrimitive(instance, context);
}

template<>
bool Serializer::Read(const json& context, unsigned int& instance)
{
    return ReadPrimitive(context, instance);
}
template<>
bool Serializer::Write(const unsigned int& instance, json& context)
{
    return WritePrimitive(instance, context);
}

template<>
bool Serializer::Read(const json& context, float& instance)
{
    return ReadPrimitive(context, instance);
}
template<>
bool Serializer::Write(const float& instance, json& context)
{
    return WritePrimitive(instance, context);
}

template<>
bool Serializer::Read(const json& context, double& instance)
{
    return ReadPrimitive(context, instance);
}
template<>
bool Serializer::Write(const double& instance, json& context)
{
    return WritePrimitive(instance, context);
}

template<>
bool Serializer::Read(const json& context, String& instance)
{
    return ReadPrimitive(context, instance);
}
template<>
bool Serializer::Write(const String& instance, json& context)
{
    return WritePrimitive(instance, context);
}


#include "_generated/serializer/all_serializer.ipp"