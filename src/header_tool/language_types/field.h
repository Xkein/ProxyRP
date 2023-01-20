#pragma once

#include "type_info.h"

class Class;

class Field : public TypeInfo
{
public:
    Field(const Cursor& cursor, const Namespace& current_namespace, Class* parent = nullptr);

    virtual ~Field() {}

    bool ShouldCompile() const;

public:

    bool IsConst;
    
    Class* Parent;

    std::string Name;
    std::string DisplayName;
    std::string Type;

    std::string Default;
};
