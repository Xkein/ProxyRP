#pragma once

#include "asset_data.h"

#include <map>

class AssetRegistry
{
public:
    template<typename AssetType>
    size_t Register(const String& url, const std::shared_ptr<AssetType>& ptr) {
        size_t hash = std::hash<String>()(url);
        Register(hash);

        return hash;
    }
    template<typename AssetType>
    void Register(size_t hash, const std::shared_ptr<AssetType>& ptr)
    {
        AssetData data(hash, ptr);
        Data[hash] = data;
    }

    template<typename AssetType>
    std::shared_ptr<AssetType> GetAsset(const String& url)
    {
        return GetAsset<AssetType>(std::hash<String>()(url);
    }
    template<typename AssetType>
    std::shared_ptr<AssetType> GetAsset(size_t hash)
    {
        if (auto iter = Data.find(hash); iter != Data.end())
        {
            return iter->second.GetData<AssetType>();
        }

        return std::shared_ptr<AssetType>();
    }

    void Clean();

private:
    std::map<size_t, AssetData> Data;
};
