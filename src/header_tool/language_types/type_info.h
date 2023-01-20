#pragma once

#include "common/namespace.h"
#include "cursor/cursor.h"
#include "meta/meta_info.h"

class TypeInfo
{
public:
    TypeInfo(const Cursor& cursor, const Namespace& current_namespace);
    virtual ~TypeInfo() {}

    const MetaInfo& GetMetaData() const;

    std::string GetSourceFile() const;

    Namespace GetCurrentNamespace() const;

    Cursor& GetCursor();

protected:
    MetaInfo MetaData;

    Namespace CurNamespace;

private:
    Cursor RootCursor;
};
