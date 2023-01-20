#include "cursor_type.h"
#include "meta/meta_utils.h"
#include "cursor.h"

CursorType::CursorType(const CXType& handle) : Handle(handle)
{
    SetupDebugInfo();
}

std::string CursorType::GetDisplayName() const
{
    return utils::to_string(clang_getTypeSpelling(Handle));
}

int CursorType::GetArgumentCount() const
{
    return clang_getNumArgTypes(Handle);
}

CursorType CursorType::GetArgument(int index) const
{
    return clang_getArgType(Handle, index);
}

CursorType CursorType::GetCanonicalType() const
{
    return clang_getCanonicalType(Handle);
}

Cursor CursorType::GetDeclaration() const
{
    return clang_getTypeDeclaration(Handle);
}

CXTypeKind CursorType::GetKind() const
{
    return Handle.kind;
}

bool CursorType::IsConst() const
{
    return clang_isConstQualifiedType(Handle);
}

void CursorType::SetupDebugInfo()
{
#ifdef DEBUG_HEADER_TOOL
    Kind = GetKind();
    TypeSpelling = GetDisplayName();
#endif
}
