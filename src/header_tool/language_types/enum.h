#pragma once

#include "type_info.h"

#include <memory>

class Class;

struct EnumConstant : public TypeInfo
{
    EnumConstant(const Cursor& cursor, const Namespace& current_namespace);

    bool ShouldCompile() const;

    std::string Name;
    std::string DisplayName;

    //union
    //{
    //    long long          SignedValue;
    //    unsigned long long UnsignedValue;
    //};
};

class Enum : public TypeInfo
{
public:
    Enum(const Cursor& cursor, const Namespace& current_namespace);

    virtual ~Enum() {}

    bool ShouldCompile() const;

public:
    bool IsEnumClass;

    std::string Name;

    std::vector<std::shared_ptr<EnumConstant>> Constants;
};
