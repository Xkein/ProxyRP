#pragma once

#include "type_info.h"

class Class;

class Method : public TypeInfo
{
public:
    Method(const Cursor& cursor, const Namespace& current_namespace, Class* parent = nullptr);

    virtual ~Method() {}

    bool ShouldCompile() const;

public:
    bool IsStatic;
    bool IsConst;

    Class* Parent;

    std::string Name;
    std::string DisplayName;
    std::string Type;

};
