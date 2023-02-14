#include "type_info.h"

TypeInfo::TypeInfo(const Cursor& cursor, const Namespace& current_namespace) :
    MetaData(cursor), CurNamespace(current_namespace), RootCursor(cursor)
{}

const MetaInfo& TypeInfo::GetMetaData() const
{
    return MetaData;
}

std::string TypeInfo::GetSourceFile() const
{
    return RootCursor.GetSourceFile();
}

Namespace TypeInfo::GetCurrentNamespace() const
{
    return CurNamespace;
}

Cursor& TypeInfo::GetCursor()
{
    return RootCursor;
}
