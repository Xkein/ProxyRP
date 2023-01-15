#pragma once

#include "platform/string.h"
#include "core/serializer/serializer.h"
#include "platform/file/file_manager.h"
#include "resource/asset/asset_registry.h"

class AssetManager
{
public:
    template <typename AssetType>
    bool LoadAsset(const String& asset_url, AssetType& out_asset)
    {
        String asset_path = GetFullPath(asset_url);
        auto text = FileManager::ReadString(asset_path.c_str());

        json config = json::parse(text);

        return false;
    }
    template<typename AssetType>
    bool SaveAsset(const AssetType& asset, const String& asset_url)
    {
        return false;
    }

private:
    String GetFullPath(const String& rel_path) const;

    std::shared_ptr<AssetRegistry> Registry;
};
