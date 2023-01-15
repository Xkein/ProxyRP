#include "world_manager.h"
#include "core/log/log_system.h"

WorldManager::~WorldManager()
{
    Clear();
}

void WorldManager::Initialize()
{
    IsWorldLoaded = false;
}

void WorldManager::Clear()
{
    CurrentActiveLevel->Unload();

    CurrentActiveLevel.reset();

    IsWorldLoaded = false;
}

void WorldManager::Tick(float delta_time)
{
    if (!IsWorldLoaded)
    {
        LoadWorld("");
    }

    std::shared_ptr<Level> active_level = CurrentActiveLevel;
    if (active_level)
    {
        active_level->Tick(delta_time);
    }
}

bool WorldManager::LoadWorld(const String& world_url)
{
    LOG_INFO("loading world: {}", world_url);

    const bool is_level_load_success = LoadLevel("");
    if (!is_level_load_success)
    {
        return false;
    }

    return true;
}

bool WorldManager::LoadLevel(const String& level_url)
{
    std::shared_ptr<Level> level = std::make_shared<Level>();

    CurrentActiveLevel = level;

    const bool is_level_load_success = level->Load(level_url);
    if (!is_level_load_success)
    {
        return false;
    }

    return false;
}
