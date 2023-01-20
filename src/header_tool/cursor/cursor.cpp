#include "cursor.h"
#include "meta/meta_utils.h"
#include <clang-c/Index.h>

Cursor::Cursor(const CXCursor& handle) : Handle(handle), Type(clang_getCursorType(Handle))
{
    SetupDebugInfo();
}

CXCursorKind Cursor::GetKind() const
{
    return Handle.kind;
}

std::string Cursor::GetSpelling() const
{
    return utils::to_string(clang_getCursorSpelling(Handle));
}

std::string Cursor::GetDisplayName() const
{
    return utils::to_string(clang_getCursorDisplayName(Handle));
}

std::string Cursor::GetSourceFile() const
{
    CXSourceRange    range = clang_Cursor_getSpellingNameRange(Handle, 0, 0);
    CXSourceLocation start = clang_getRangeStart(range);

    CXFile file;
    uint32_t line, column, offset;

    clang_getFileLocation(start, &file, &line, &column, &offset);

    return utils::to_string(clang_getFileName(file));
}

bool Cursor::IsDefinition() const
{
    return clang_isCursorDefinition(Handle);
}

const CursorType& Cursor::GetType() const
{
    return Type;
}

std::vector<Cursor> Cursor::GetChildren() const
{
    std::vector<Cursor> children;

    auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data) {
        auto container = static_cast<std::vector<Cursor>*>(data);

        container->emplace_back(cursor);

        if (cursor.kind == CXCursor_LastPreprocessing)
            return CXChildVisit_Break;

        return CXChildVisit_Continue;
    };

    clang_visitChildren(Handle, visitor, &children);

    return children;
}

void Cursor::VisitChildren(CXCursorVisitor visitor, void* data)
{
    clang_visitChildren(Handle, visitor, data);
}

void Cursor::SetupDebugInfo()
{
#ifdef DEBUG_HEADER_TOOL
    Kind               = GetKind();
    Spelling           = GetSpelling();
    DisplayName        = GetDisplayName();
    SourceFileLocation = GetSourceFile();
    Children           = GetChildren();
#endif
}
