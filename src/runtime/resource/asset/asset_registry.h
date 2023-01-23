#pragma once

#include "asset_data.h"

#include <map>

class AssetRegistry
{
public:
    template<typename AssetType>
    void Register(const String& url, const std::shared_ptr<AssetType>& ptr) {
        AssetData data(url, ptr);
        Data[url] = data;
    }

    template<typename AssetType>
    std::shared_ptr<AssetType> GetAsset(const String& url)
    {
        if (auto iter = Data.find(url); iter != Data.end())
        {
            return iter->second.GetData<AssetType>();
        }

        return std::shared_ptr<AssetType>();
    }

private:
    std::map<String, AssetData> Data;
};
