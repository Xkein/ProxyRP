#include "method.h"

Method::Method(const Cursor& cursor, const Namespace& current_namespace, Class* parent) :
    TypeInfo(cursor, current_namespace), Parent(parent), Name(cursor.GetSpelling()),
    IsStatic(cursor.IsMethodStatic()), IsConst(cursor.IsMethodConst())
{}

bool Method::ShouldCompile() const
{
    return MetaData.GetFlag(MetaFlags::Reflected);
}

