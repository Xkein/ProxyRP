#pragma once

#include <memory>

class LogSystem;
class InputSystem;
class WindowSystem;
class RenderSystem;
class AssetManager;
class WorldManager;

extern std::shared_ptr<LogSystem>    GLogSystem;
extern std::shared_ptr<InputSystem>  GInputSystem;
extern std::shared_ptr<WindowSystem> GWindowSystem;
extern std::shared_ptr<RenderSystem> GRenderSystem;
extern std::shared_ptr<AssetManager> GAssetManager;
extern std::shared_ptr<WorldManager> GWorldManager;

