#include "asset_manager.h"

#include <filesystem>

String AssetManager::GetFullPath(const String& rel_path) const
{
    return std::filesystem::absolute(rel_path).string();
}
