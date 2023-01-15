
#include <fstream>
#include <nlohmann/json.hpp>

#include "core/log/log_system.h"
#include "platform/platform.h"
#include "engine_config.h"

using json = nlohmann::json;

void EngineConfig::Load(const std::filesystem::path& config_path)
{
    if (!std::filesystem::exists(config_path))
    {
        LOG_INFO("unable to load config: {}", config_path.string());
        return;
    }

    LOG_INFO("load config: {}", config_path.string());

    std::ifstream file(config_path);
    json          config = json::parse(file);

    VSync = config["VSync"].get<bool>();
}


