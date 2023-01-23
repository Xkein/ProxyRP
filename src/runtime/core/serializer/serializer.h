#pragma once

#include "core/reflection/reflection.h"
#include "platform/string.h"
#include "core/macro.h"

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
        String type_name = context[_SERIALIZER_TYPE_NAME].get<String>();
        instance.SetType(rttr::type::get_by_name(type_name));
        return ReadPointer(context, instance.m_instance);
    }

    template<typename T>
    static bool Write(const Instance<T>& instance, json& context)
    {
        StringView         type_name           = instance.GetTypeName();
        ReflectionInstance reflection_instance = instance;

        json instance_json;
        bool ret                       = Type::Write(reflection_instance, instance_json);
        context[_SERIALIZER_TYPE_NAME] = type_name;
        context[_SERIALIZER_CONTEXT]   = instance_json;
        return ret;
    }

	template<typename T>
    static bool Read(const json& context, T& instance)
    {
        if constexpr (std::is_pointer_v<T>)
        {
            return ReadPointer(context, (T)instance);
        }
        else
        {
            ASSERT(std::is_pointer_v<T>);
        }

        return false;
    }

    template<typename T>
    static bool Write(const T& instance, json& context)
    {
        if constexpr (std::is_pointer_v<T>)
        {
            return WritePointer(context, (T)instance);
        }
        else
        {
            ASSERT(std::is_pointer_v<T>);
        }

        return false;
    }

private:
    
	template<typename T>
    static bool ReadPointer(const json& context, T*& instance)
    {
        String type_name = context[_SERIALIZER_TYPE_NAME].get<String>();
        ASSERT(!type_name.empty());
        if (type_name[0] == '*')
        {
            instance = new T();
            return Serializer::Read(context[_SERIALIZER_CONTEXT], *instance);
        }
        else
        {
            instance = (T*)Type::New(rttr::type::get_by_name(type_name), context[_SERIALIZER_CONTEXT]).GetPtr();
            return instance != nullptr;
        }
    }

    template<typename T>
    static bool WritePointer(const T* instance, json& context)
    {
        json instance_json;
        bool ret = Serializer::Write(*instance, instance_json);
        context[_SERIALIZER_TYPE_NAME] = "*";
        context[_SERIALIZER_CONTEXT]   = instance_json;
        return ret;
    }
};

// Serializer for base types

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

#ifndef __REFLECTION_HEADER_TOOL__
#include "_generated/serializer/all_serializer.h"
#endif // !__REFLECTION_HEADER_TOOL__
