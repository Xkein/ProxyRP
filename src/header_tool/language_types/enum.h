#pragma once

#include "type_info.h"

class Class;

class Enum : public TypeInfo
{
public:
    Enum(const Cursor& cursor, const Namespace& current_namespace);

    virtual ~Enum() {}

    bool ShouldCompile() const;

public:
    bool IsEnumClass();

    std::string Name;
    std::string DisplayName;
    std::string EnumBase;

};
