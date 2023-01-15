#include "engine.h"
#include "core/log/log_system.h"
#include "function/framework/world/world_manager.h"
#include "function/global/global_context.h"
#include "function/render/render_system.h"
#include "function/input/input_system.h"
#include "function/ui/window_system.h"
#include "platform/platform.h"
#include "resource/config/engine_config.h"

#include <filesystem>

EngineConfig g_engine_config;

void Engine::Start(const String& config_dir)
{
    std::filesystem::path dir = config_dir;

    g_engine_config.Load(dir / "engine_config.json");

    StartSystems(config_dir);

	LOG_INFO("engine start.");
}

void Engine::Shutdown() {
	LOG_INFO("engine shutdown.");

    ShutdownSystems();
}

void Engine::Run()
{
    //extern void test_vulkan();
    //test_vulkan();

    while (!GWindowSystem->ShouldClose())
    {
        float delta_time = CalculateDeltaTime();
        Tick(delta_time);
    }
}

void Engine::StartSystems(const String& config_dir)
{
    GWindowSystem = std::make_shared<WindowSystem>();
    WindowCreateInfo window_create_info {.Width = 1024, .Height = 960, .Title = "Proxy Rendering Process", .IsFullScreen = false};
    GWindowSystem->Initialize(window_create_info);

}

void Engine::ShutdownSystems()
{
    GWindowSystem.reset();
}

void Engine::Tick(float delta_time)
{
    LogicTick(delta_time);

    RenderTick(delta_time);

    GWindowSystem->PollEvents();
}

void Engine::LogicTick(float delta_time)
{
    GWorldManager->Tick(delta_time);
    GInputSystem->Tick();
}

void Engine::RenderTick(float delta_time)
{
    GRenderSystem->Tick(delta_time);
}

float Engine::CalculateDeltaTime()
{
    using namespace std::chrono;

    float delta_time;
    steady_clock::time_point tick_time_point = steady_clock::now();
    duration<float>          time_span       = duration_cast<duration<float>>(tick_time_point - LastTickTimePoint);
    delta_time                               = time_span.count();

    LastTickTimePoint = tick_time_point;

    return delta_time;
}

