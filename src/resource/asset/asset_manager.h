#pragma once

#include "platform/string.h"


class AssetManager
{
public:
    template <typename AssetType>
    static bool LoadAsset(const String& asset_url, AssetType& out_asset)
    {

    }
    template<typename AssetType>
    static bool SaveAsset(const AssetType& asset, const String& asset_url)
    {

    }
};
