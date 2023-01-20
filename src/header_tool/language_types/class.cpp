#include "class.h"
#include "meta/meta_utils.h"

BaseClass::BaseClass(const Cursor& cursor) : Name(cursor.GetType().GetDisplayName()) {}

Class::Class(const Cursor& cursor, const Namespace& current_namespace) :
    TypeInfo(cursor, current_namespace), Name(cursor.GetDisplayName()),
    QualifiedName(cursor.GetType().GetDisplayName())
{
    DisplayName = QualifiedName;

    utils::replace_all(Name, " ", "");

    for (auto& child : cursor.GetChildren())
    {
        switch (child.GetKind())
        {
            case CXCursor_CXXBaseSpecifier:
                BaseClasses.emplace_back(new BaseClass(child));
                break;
            case CXCursor_FieldDecl:
                Fields.emplace_back(new Field(child, current_namespace, this));
                break;
            default:
                break;
        }
    }
}

bool Class::ShouldCompile() const
{
    return ShouldCompileFields();
}

bool Class::ShouldCompileFields() const
{
    return MetaData.GetFlag(MetaFlags::Reflected);
}

std::string Class::GetClassName() const
{
    return Name;
}

bool Class::IsAccessible() const
{
    return true;
}
