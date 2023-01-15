#include "reflection.h"

rttr::type Type::InvalidType = rttr::type::get<void>();

ReflectionInstance Type::New(rttr::type type, const json& context)
{
    return ReflectionInstance();
}

json Type::Write(const ReflectionInstance& instance)
{
    return json();
}
