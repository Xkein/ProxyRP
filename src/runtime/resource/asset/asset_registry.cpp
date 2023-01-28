#include "asset_registry.h"

#include <vector>

void AssetRegistry::Clean()
{
    std::vector<size_t> expired_asset;
    for (const auto& [hash, asset_data] : Data)
    {
        if (asset_data.Data.expired())
        {
            expired_asset.emplace_back(hash);
        }
    }

    for (size_t expired_hash : expired_asset)
    {
        Data.erase(expired_hash);
    }
}
