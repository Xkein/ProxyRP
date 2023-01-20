#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class Cursor;

namespace MetaFlags
{
    constexpr auto Reflected = "Reflected";
}

class MetaInfo
{
public:
    MetaInfo(const Cursor& cursor);

    std::string GetProperty(const std::string& key) const;

    bool GetFlag(const std::string& key) const;

private:
    typedef std::pair<std::string, std::string> Property;
    std::vector<Property> ExtractProperties(const Cursor& cursor) const;

    std::unordered_map<std::string, std::string> Properties;

};
