#pragma once

#include "serializer_base.h"

// Serializer for primitive types

template<>
bool Serializer::Read(const json& context, bool& instance);
template<>
bool Serializer::Write(const bool& instance, json& context);

template<>
bool Serializer::Read(const json& context, char& instance);
template<>
bool Serializer::Write(const char& instance, json& context);

template<>
bool Serializer::Read(const json& context, unsigned char& instance);
template<>
bool Serializer::Write(const unsigned char& instance, json& context);

template<>
bool Serializer::Read(const json& context, short& instance);
template<>
bool Serializer::Write(const short& instance, json& context);

template<>
bool Serializer::Read(const json& context, unsigned short& instance);
template<>
bool Serializer::Write(const unsigned short& instance, json& context);

template<>
bool Serializer::Read(const json& context, int& instance);
template<>
bool Serializer::Write(const int& instance, json& context);

template<>
bool Serializer::Read(const json& context, unsigned int& instance);
template<>
bool Serializer::Write(const unsigned int& instance, json& context);

template<>
bool Serializer::Read(const json& context, float& instance);
template<>
bool Serializer::Write(const float& instance, json& context);

template<>
bool Serializer::Read(const json& context, double& instance);
template<>
bool Serializer::Write(const double& instance, json& context);

template<>
bool Serializer::Read(const json& context, String& instance);
template<>
bool Serializer::Write(const String& instance, json& context);

#include "core/math/math_serializer.h"

#if not defined(__REFLECTION_HEADER_TOOL__)
#include "_generated/serializer/all_serializer.h"
#endif
