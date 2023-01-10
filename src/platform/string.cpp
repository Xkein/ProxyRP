#include "string.h"

#include <codecvt>
#include <locale>

std::wstring to_wstring(const std::string& str)
{
    std::mbstate_t mb;
    std::wstring   result(str.size(), L'\0');
    const char*    from_next;
    wchar_t*       to_next;

    auto& f = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(std::locale());
    f.in(mb, &str[0], &str[str.size()], from_next,
             &result[0], &result[result.size()], to_next);
    result.resize(to_next - &result[0]);

    return std::move(result);
}

std::string to_string(const std::wstring& str)
{
    std::mbstate_t mb;
    std::string   result(str.size(), '\0');
    const wchar_t* from_next;
    char*       to_next;

    auto& f = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(std::locale());
    f.out(mb, &str[0], &str[str.size()], from_next, &result[0], &result[result.size()], to_next);
    result.resize(to_next - &result[0]);

    return std::move(result);
}
