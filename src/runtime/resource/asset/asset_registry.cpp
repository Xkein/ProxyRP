#include "asset_registry.h"

#include <vector>

void AssetRegistry::Clean()
{
    std::vector<String> expired_urls;
    for (const auto& [url, asset_data] : Data)
    {
        if (asset_data.Data.expired())
        {
            expired_urls.emplace_back(url);
        }
    }

    for (const String& expired_url : expired_urls)
    {
        Data.erase(expired_url);
    }
}
