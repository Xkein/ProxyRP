#pragma once

#include "core/macro.h"
#include "platform/string.h"

class Engine
{
public:
    void Start(const String& config_dir);
    void Exit();

    void Run();
private:
    void Tick(float delta_time);

    void LogicTick(float delta_time);
    void RenderTick(float delta_time);

};

