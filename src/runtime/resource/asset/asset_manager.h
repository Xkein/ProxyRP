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
        auto   text       = FileManager::ReadString(asset_path.c_str());

        json asset_json = json::parse(text);

        return Serializer::Read(asset_json, out_asset);
    }
    template<typename AssetType>
    bool SaveAsset(const AssetType& asset, const String& asset_url)
    {
        String asset_path = GetFullPath(asset_url);

        json asset_json;
        Serializer::Write(asset, asset_json);

        FileManager::WriteString(asset_url, asset_json.dump());

        return true;
    }

private:
    String GetFullPath(const String& rel_path) const;

    std::shared_ptr<AssetRegistry> Registry;
};
