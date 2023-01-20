#pragma once

#include <clang-c/Index.h>
#include <string>

class Cursor;

class CursorType
{
public:
    CursorType(const CXType& handle);

    std::string GetDisplayName() const;

    int GetArgumentCount() const;

    CursorType GetArgument(int index) const;

    CursorType GetCanonicalType() const;

    Cursor GetDeclaration() const;

    CXTypeKind GetKind() const;

    bool IsConst() const;

private:
    void SetupDebugInfo();

    CXType Handle;

#ifdef DEBUG_HEADER_TOOL
    CXTypeKind  Kind;
    std::string TypeSpelling;
#endif // NDEBUG
};
