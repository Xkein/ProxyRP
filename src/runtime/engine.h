#pragma once

#include "core/macro.h"
#include "platform/string.h"

#include <chrono>

class Engine
{
public:
    void Start();
    void Shutdown();

    void Run();
private:
    void StartSystems();
    void ShutdownSystems();

    void Tick(float delta_time);

    void LogicTick(float delta_time);
    void RenderTick(float delta_time);

    float CalculateDeltaTime();

    std::chrono::steady_clock::time_point LastTickTimePoint {std::chrono::steady_clock::now()};
};

