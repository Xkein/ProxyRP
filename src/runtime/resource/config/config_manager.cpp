#include "config_manager.h"
#include "function/global/global_context.h"
#include "resource/asset/asset_manager.h"

void ConfigManager::Initialize()
{
    GAssetManager->LoadAsset("asset/config/global.config.json", Global);
    GAssetManager->LoadAsset("asset/config/engine.config.json", Engine);
    GAssetManager->LoadAsset("asset/config/graphics.config.json", Graphics);
}
