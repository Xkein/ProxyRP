
#include <fstream>
#include <nlohmann/json.hpp>

#include "core/logger.h"
#include "platform/platform.h"
#include "engine_config.h"

using json = nlohmann::json;

void EngineConfig::Load(const std::filesystem::path& config_path)
{
    if (!std::filesystem::exists(config_path))
    {
        LOG_INFO(TEXT("unable to load config: %s"), config_path.string().c_str());
        return;
    }

    LOG_INFO(TEXT("load config: %s"), config_path.string().c_str());

    std::ifstream file(config_path);
    json          config = json::parse(file);

    VSync = config["VSync"].get<bool>();
}


