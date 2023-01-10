#include <filesystem>

#include "engine.h"
#include "core/logger.h"
#include "platform/platform.h"
#include "resource/config/engine_config.h"

EngineConfig g_engine_config;

void Engine::Start(const String& config_dir) {
	LOG_INFO(TEXT("engine start."));

    std::filesystem::path dir = config_dir;

    g_engine_config.Load(dir / "engine_config.json");

}

void Engine::Exit() {


	LOG_INFO(TEXT("engine exit."));
}

void Engine::Run()
{
    extern void test_vulkan();
    test_vulkan();


}

void Engine::Tick(float delta_time)
{
    LogicTick(delta_time);

    RenderTick(delta_time);
}

void Engine::LogicTick(float delta_time)
{

}

void Engine::RenderTick(float delta_time)
{

}

