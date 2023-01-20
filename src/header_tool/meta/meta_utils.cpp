#include "meta_utils.h"

#include <fstream>
#include <sstream>
#include <ranges>
#include <filesystem>

std::vector<std::string> utils::split(std::string_view input, std::string_view pat)
{
    std::vector<std::string> result;

    for (const auto& str : std::views::split(input, pat))
    {
        result.emplace_back(str.begin(), str.end());
    }

    return result;
}

std::string utils::trim(std::string_view input, std::string_view trim_chars)
{
    size_t left = input.find_first_not_of(trim_chars);
    if (left == std::string_view::npos)
    {
        return std::string();
    }
    size_t right = input.find_last_not_of(trim_chars);

    return std::string(input.begin() + left, input.begin() + right + 1);
}

void utils::replace_all(std::string& resource_str, std::string_view sub_str, std::string_view new_str)
{
    size_t pos = 0;
    while ((pos = resource_str.find(sub_str)) != std::string::npos)
    {
        resource_str.replace(pos, sub_str.length(), new_str);
    }
}

std::string utils::get_filename(std::string_view path)
{
    return std::filesystem::path(path).filename().string();
}

std::string utils::change_extension(const std::string& path, const std::string_view ext)
{
    return std::filesystem::path(path).replace_extension(ext).string();
}

std::string utils::get_relative_path(std::string_view from, std::string_view to)
{
    return std::filesystem::relative(to, from).string();
}

std::string utils::load_file(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string text(buffer.str());
    return text;
}

void utils::save_file(const std::string& content, const std::string& output_file)
{
    std::filesystem::path out_path(output_file);
    if (std::filesystem::exists(out_path.parent_path()))
    {
        std::filesystem::create_directories(out_path.parent_path());
    }

    std::ofstream file(output_file);

    file << content << std::endl;
    file.flush();
    file.close();
}

std::string utils::to_string(const CXString& str)
{
    if (str.data == nullptr)
        return "";
    auto cstr = clang_getCString(str);

    std::string result = cstr;

    clang_disposeString(str);

    return result;
}
