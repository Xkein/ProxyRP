#pragma once

#include "type_info.h"
#include "field.h"
#include "method.h"

#include <memory>

struct BaseClass
{
    BaseClass(const Cursor& cursor);

    std::string Name;
};

class Class : public TypeInfo
{
    friend class Field;
    friend class MetaParser;

public:
    Class(const Cursor& cursor, const Namespace& current_namespace);

    bool ShouldCompile() const;

    bool ShouldCompileFields() const;

    std::string GetClassName() const;

    std::vector<std::shared_ptr<BaseClass>> BaseClasses;

    std::string Name;
    std::string DisplayName;
    std::string QualifiedName;

    std::vector<std::shared_ptr<Field>>  Fields;
    std::vector<std::shared_ptr<Method>> Methods;

    bool IsAccessible() const;

};
