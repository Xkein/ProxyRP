#pragma once

#include "core/reflection/reflection.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define _SERIALIZER_TYPE_NAME "$type_name"
#define _SERIALIZER_CONTEXT   "$context"

class Serializer
{
public:
    template<typename T>
    static bool Read(const json& context, Instance<T>& instance)
    {
    }

    template<typename T>
    static bool Write(const Instance<T>& instance, json& context)
    {
    }

	template<typename T>
    static bool Read(const json& context, T& instance)
    {
        if constexpr (std::is_pointer_v<T>)
        {
            ReadPointer(context, (T)instance);
        }
    }

    template<typename T>
    static bool Write(const T& instance, json& context)
    {
        if constexpr (std::is_pointer_v<T>)
        {
            WritePointer(context, (T)instance);
        }
    }

private:
    
	template<typename T>
    static bool ReadPointer(const json& context, T* instance)
    {
    }

    template<typename T>
    static bool WritePointer(const T* instance, json& context)
    {
    }
};

template<>
bool Serializer::Read(const json& context, bool& instance);
template<>
bool Serializer::Write(const bool& instance, json& context);

template<>
bool Serializer::Read(const json& context, char& instance);
template<>
bool Serializer::Write(const char& instance, json& context);

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

