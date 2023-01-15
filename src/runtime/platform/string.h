#pragma once

#include <string>

std::wstring to_wstring(const std::string& str);

std::string  to_string(const std::wstring& str);

typedef char        Char;
typedef std::string String;

#ifndef TEXT
//#define TEXT(x) L##x
#define TEXT(x) x
#endif // !TEXT
