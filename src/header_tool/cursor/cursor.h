#pragma once

#include "cursor_type.h"
#include <vector>

class Cursor
{
public:
	Cursor(const CXCursor& handle);

	CXCursorKind GetKind() const;

	std::string GetSpelling() const;
    std::string GetDisplayName() const;

	std::string GetSourceFile() const;

	bool IsDefinition() const;

    bool IsMethodStatic() const;
    bool IsMethodConst() const;

    bool IsEnumClass() const;

	const CursorType& GetType() const;

	std::vector<Cursor> GetChildren() const;
    void                VisitChildren(CXCursorVisitor visitor, void* data = nullptr);

private:
    void SetupDebugInfo();

    CXCursor Handle;
    CursorType Type;

#ifdef DEBUG_HEADER_TOOL
    CXCursorKind        Kind;
    std::string         Spelling;
    std::string         DisplayName;
    std::string         SourceFileLocation;
    std::vector<Cursor> Children;
#endif // DEBUG
};
