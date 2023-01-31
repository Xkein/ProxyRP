#include "asset_manager.h"
#include "function/global/global_context.h"
#include "resource/config/config_manager.h"

#include <filesystem>

String AssetManager::GetFullPath(StringView rel_path) const
{
    return std::filesystem::absolute(GConfigManager->EngineRootPath / rel_path).string();
}
