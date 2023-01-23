#include "reflection.h"
#include "reflection_register.h"
#include "core/serializer/serializer.h"

rttr::type Type::InvalidType = rttr::type::get<void>();

std::map<StringView, __SerializerMethods> SerializerMethods;

void TypeMetaRegister::__RegisterSerializerMethods(const StringView& name, __SerializerMethods methods)
{
    SerializerMethods[name] = methods;
}

ReflectionInstance Type::New(const rttr::type& type, const json& context)
{
    if (!type.is_valid())
    {
        return ReflectionInstance();
    }

    //auto reflection_type = GetReflectionType(type);
    //auto  method = reflection_type.get_method("ConstructWithJson");
    //void* ptr    = method.invoke({}, context).get_value<void*>();

    StringView name(type.get_name().begin(), type.get_name().end());
    void*      ptr = SerializerMethods[name].ConstructWithJson(context);
    return ReflectionInstance(type, ptr);
}

bool Type::Write(const ReflectionInstance& instance, json& context)
{
    auto& type = instance.GetType();
    if (!type.is_valid())
    {
        return false;
    }

    //auto  reflection_type = GetReflectionType(type);
    //auto  method          = reflection_type.get_method("WriteJson");
    //return method.invoke({}, (void*)(const void*)instance, context).get_value<bool>();

    StringView name(type.get_name().begin(), type.get_name().end());
    return SerializerMethods[name].WriteJson((void*)instance.GetPtr(), context);
}
