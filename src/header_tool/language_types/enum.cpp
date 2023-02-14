#include "enum.h"
#include "meta/meta_utils.h"

EnumConstant::EnumConstant(const Cursor& cursor, const Namespace& current_namespace) :
    TypeInfo(cursor, current_namespace), Name(cursor.GetSpelling())
{
    DisplayName = utils::trim(MetaData.GetProperty("DisplayName"), " \"");
}

bool EnumConstant::ShouldCompile() const
{
    return DisplayName != "";
}

Enum::Enum(const Cursor& cursor, const Namespace& current_namespace) :
    TypeInfo(cursor, current_namespace), Name(cursor.GetSpelling()), IsEnumClass(cursor.IsEnumClass())
{
    for (auto& child : cursor.GetChildren())
    {
        switch (child.GetKind())
        {
            case CXCursor_EnumConstantDecl:
                Constants.emplace_back(new EnumConstant(child, current_namespace));
                break;
        }
    }
}

bool Enum::ShouldCompile() const
{
    return MetaData.GetFlag(MetaFlags::Reflected);
}
