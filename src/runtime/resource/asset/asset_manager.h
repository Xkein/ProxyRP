#pragma once

#include "core/log/log_system.h"
#include "platform/string.h"
#include "core/serializer/serializer.h"
#include "platform/file/file_manager.h"
#include "resource/asset/asset_registry.h"

class AssetManager
{
public:
    /**
     * Load asset without caching. This could be resulting in redundant loading.
     * 
     * \param asset_url
     * \param out_asset
     * \return 
     */
    template <typename AssetType>
    bool LoadAsset(StringView asset_url, AssetType& out_asset)
    {
        String asset_path = GetFullPath(asset_url);
        auto   text       = FileManager::ReadString(asset_path);
        if (text.empty())
        {
            LOG_ERROR("could not open asset: {}", asset_url);
            return false;
        }

        json asset_json = json::parse(text);
        bool ret        = Serializer::Read(asset_json, out_asset);

        return ret;
    }
    /**
     * Save asset.
     * 
     * \param asset
     * \param asset_url
     * \return 
     */
    template<typename AssetType>
    bool SaveAsset(const AssetType& asset, StringView asset_url)
    {
        String asset_path = GetFullPath(asset_url);

        json asset_json;
        bool ret = Serializer::Write(asset, asset_json);
        ret &= FileManager::WriteString(asset_url, asset_json.dump());

        return ret;
    }

    /**
     * Load asset with caching mechanic.
     * 
     * \param asset_url
     * \param out_asset
     * \return 
     */
    template<typename AssetType>
    bool LoadAsset(StringView asset_url, std::shared_ptr<AssetType>& out_asset)
    {
        std::shared_ptr<AssetType> cached_asset = Registry->GetAsset<AssetType>(asset_url);
        if (cached_asset)
        {
            out_asset = cached_asset;
            return true;
        }

        cached_asset = std::make_shared<AssetType>();

        bool ret = LoadAsset(asset_url, *cached_asset);
        Registry->Register(asset_url, cached_asset);

        out_asset = cached_asset;
        return ret;
    }
    /**
     * Save asset.
     * 
     * \param asset
     * \param asset_url
     * \return 
     */
    template<typename AssetType>
    bool SaveAsset(const std::shared_ptr<AssetType>& asset, StringView asset_url)
    {
        return Save(*asset, asset_url);
    }

private:
    String GetFullPath(StringView rel_path) const;

    std::shared_ptr<AssetRegistry> Registry;
};

extern std::shared_ptr<AssetManager> GAssetManager;

