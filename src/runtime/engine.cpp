#include "engine.h"
#include "core/log/log_system.h"
#include "core/reflection/reflection_register.h"
#include "function/framework/world/world_manager.h"
#include "function/global/global_context.h"
#include "function/render/render_system.h"
#include "function/input/input_system.h"
#include "function/ui/window_system.h"
#include "platform/platform.h"
#include "resource/asset/asset_manager.h"
#include "resource/config/config_manager.h"

#include <filesystem>

void Engine::Start()
{
    TypeMetaRegister::Register();

    StartSystems();

	LOG_INFO("engine start.");
}

void Engine::Shutdown() {
	LOG_INFO("engine shutdown.");

    ShutdownSystems();

    TypeMetaRegister::Unregister();
}

void Engine::Run()
{
    while (!GWindowSystem->ShouldClose())
    {
        float delta_time = CalculateDeltaTime();
        Tick(delta_time);
    }
}

void Engine::StartSystems()
{
    GLogSystem = std::make_shared<LogSystem>();

    GConfigManager = std::make_shared<ConfigManager>();
    GConfigManager->Initialize();

    GAssetManager = std::make_shared<AssetManager>();

    GWorldManager = std::make_shared<WorldManager>();
    GWorldManager->Initialize();

    GInputSystem = std::make_shared<InputSystem>();
    GInputSystem->Initialize();

    GWindowSystem = std::make_shared<WindowSystem>();
    WindowCreateInfo window_create_info {.Width = 1024, .Height = 512, .Title = "Proxy Rendering Process", .IsFullScreen = false};
    GWindowSystem->Initialize(window_create_info);

    GRenderSystem = std::make_shared<RenderSystem>();
    RenderSystemInitInfo render_init_info;
    render_init_info.WindowSystem = GWindowSystem;
    GRenderSystem->Initialize(render_init_info);

    ASSERT(GLogSystem);
    ASSERT(GConfigManager);
    ASSERT(GInputSystem);
    ASSERT(GWindowSystem);
    ASSERT(GRenderSystem);
    ASSERT(GAssetManager);
    ASSERT(GWorldManager);
}

void Engine::ShutdownSystems()
{
    GRenderSystem->Clear();
    GRenderSystem.reset();

    GWindowSystem.reset();

    GWorldManager->Clear();
    GWorldManager.reset();

    GInputSystem->Clear();
    GInputSystem.reset();

    GAssetManager.reset();

    GConfigManager.reset();

    GLogSystem.reset();
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

