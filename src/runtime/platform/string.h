#pragma once

#include <string>
#include <string_view>

std::wstring to_wstring(const std::string& str);

std::string  to_string(const std::wstring& str);

typedef char             Char;
typedef std::string      String;
typedef std::string_view StringView;

#ifndef TEXT
//#define TEXT(x) L##x
#define TEXT(x) x
#endif // !TEXT
