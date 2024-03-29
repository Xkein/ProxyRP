#include "config_manager.h"
#include "function/global/global_context.h"
#include "resource/asset/asset_manager.h"


void ConfigManager::Initialize()
{
    std::filesystem::path root_path = std::filesystem::current_path();

    EngineRootPath  = root_path;
    AssetPath       = root_path / "asset";
    ShaderPath      = root_path / "shaders";
    ShaderCachePath = root_path / "shaders" / "cache";
    if (!std::filesystem::exists(ShaderCachePath))
    {
        std::filesystem::create_directory(ShaderCachePath);
    }

    GAssetManager->LoadAsset("asset/config/global.config.json", Global);
    GAssetManager->LoadAsset("asset/config/engine.config.json", Engine);
    GAssetManager->LoadAsset("asset/config/graphics.config.json", Graphics);
}
