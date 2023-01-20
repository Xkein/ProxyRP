#include "meta_info.h"
#include "cursor/cursor.h"
#include "meta/meta_utils.h"

MetaInfo::MetaInfo(const Cursor& cursor)
{
    for (auto& child : cursor.GetChildren())
    {
        if (child.GetKind() != CXCursor_AnnotateAttr)
            continue;

        for (auto& [key, value] : ExtractProperties(child))
        {
            Properties[key] = value;
        }
    }
}

std::string MetaInfo::GetProperty(const std::string& key) const
{
    auto it = Properties.find(key);
    if (it == Properties.end())
        return "";

    return it->second;
}

bool MetaInfo::GetFlag(const std::string& key) const
{
    return Properties.contains(key);
}

std::vector<MetaInfo::Property> MetaInfo::ExtractProperties(const Cursor& cursor) const
{
    std::vector<Property> list;

    auto property_list = cursor.GetDisplayName();

    auto properties = utils::split(property_list, ",");

    static const std::string white_space_string = " \t\r\n";

    for (auto& property_item : properties)
    {
        auto pair = utils::split(property_item, "=");
        auto key  = utils::trim(pair[0], white_space_string);
        if (key.empty())
            continue;
        list.emplace_back(key, pair.size() > 1 ? utils::trim(pair[1], white_space_string) : "");
    }

    return list;
}

