#include "field.h"
#include "meta/meta_utils.h"

Field::Field(const Cursor& cursor, const Namespace& current_namespace, Class* parent) :
    TypeInfo(cursor, current_namespace), IsConst(cursor.GetType().IsConst()), Parent(parent),
    Name(cursor.GetSpelling()), Type(cursor.GetType().GetDisplayName())
{
    DisplayName = Name;

    utils::replace_all(Type, " ", "");

    Default = utils::trim(MetaData.GetProperty("default"), "\"");
}

bool Field::ShouldCompile() const
{
    return MetaData.GetFlag(MetaFlags::Reflected);
}

