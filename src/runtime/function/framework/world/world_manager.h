#pragma once

#include "function/framework/level/level.h"

class WorldManager
{

public:
    ~WorldManager();

    void Initialize();
    void Clear();

    void Tick(float delta_time);

    std::weak_ptr<Level> GetCurrentLevel() const
    {
        return CurrentActiveLevel;
    }

private:
    bool LoadWorld(const String& world_url);
    bool LoadLevel(const String& level_url);

    bool IsWorldLoaded {false};

    std::shared_ptr<Level> CurrentActiveLevel;
};
