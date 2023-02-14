#pragma once

#include <clang-c/CXString.h>
#include <vector>
#include <string>
#include <string_view>

namespace utils
{
    std::vector<std::string> split(std::string_view input, std::string_view pat);
    std::string              trim(std::string_view input, std::string_view trim_chars);
    void                     replace_all(std::string& resource_str, std::string_view sub_str, std::string_view new_str);
    std::string              get_filename(std::string_view path);
    std::string              change_extension(const std::string& path, const std::string_view ext);
    std::string              get_relative_path(std::string_view from, std::string_view to);

    std::string load_file(const std::string& path);
    void        save_file(const std::string& content, const std::string& output_file);

    std::string to_string(const CXString& str);
}

