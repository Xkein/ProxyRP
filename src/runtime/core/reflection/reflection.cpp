#include "reflection.h"
#include "core/serializer/serializer.h"

rttr::type Type::InvalidType = rttr::type::get<void>();

std::map<StringView, std::shared_ptr<rttr::type>> ReflectionTypeMap;

rttr::type GetReflectionType(rttr::type type) {
    StringView name(type.get_name().begin(), type.get_name().end());
    if (auto iter = ReflectionTypeMap.find(name); iter != ReflectionTypeMap.end())
        return *iter->second;

    String     reflection_type_name = "__ReflectionType";
    reflection_type_name += name;
    rttr::type reflection_type = rttr::type::get_by_name(reflection_type_name);
    ReflectionTypeMap[name]    = std::make_shared<rttr::type>(reflection_type);
    return reflection_type;
}

ReflectionInstance Type::New(const rttr::type& type, const json& context)
{
    if (!type.is_valid())
    {
        return ReflectionInstance();
    }

    auto reflection_type = GetReflectionType(type);
    auto  method = reflection_type.get_method("ConstructWithJson");
    void* ptr    = method.invoke({}, context).get_value<void*>();
    return ReflectionInstance(type, ptr);
}

bool Type::Write(const ReflectionInstance& instance, json& context)
{
    return false;
}
