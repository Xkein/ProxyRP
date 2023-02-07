#include "input_system.h"
#include "function/global/global_context.h"
#include "function/ui/window_system.h"
#include "function/render/render_system.h"
#include "function/render/render_camera.h"

#include <GLFW/glfw3.h>
#include <memory>
#include <array>

void InputSystem::OnKey(int key, int scancode, int action, int mods)
{
    OnKeyInGameMode(key, scancode, action, mods);
}

void InputSystem::OnCursorPos(double x, double y)
{
    if (GWindowSystem->GetFocusMode())
    {
        CursorDeltaX = LastCursorX - x;
        CursorDeltaY = LastCursorY - y;
    }

    LastCursorX = x;
    LastCursorY = y;
}

void InputSystem::Initialize()
{
    std::shared_ptr<WindowSystem> window_system = GWindowSystem;

    window_system->RegisterOnKeyFunc(std::bind(&InputSystem::OnKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

    window_system->RegisterOnCursorPosFunc(std::bind(&InputSystem::OnCursorPos, this, std::placeholders::_1, std::placeholders::_2));
}

void InputSystem::Clear()
{
    CursorDeltaX = 0;
    CursorDeltaY = 0;
}

void InputSystem::Tick()
{
    CalculateCursorDeltaAngles();
    Clear();

    std::shared_ptr<WindowSystem> window_system = GWindowSystem;
    if (window_system->GetFocusMode())
    {
        Command.Invalid = false;
    }
    else
    {
        Command.Invalid = true;
    }
}

void InputSystem::OnKeyInGameMode(int key, int scancode, int action, int mods)
{
    Command.Jump = false;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_A:
                Command.Left = true;
                break;
            case GLFW_KEY_S:
                Command.Backward = true;
                break;
            case GLFW_KEY_W:
                Command.Forward = true;
                break;
            case GLFW_KEY_D:
                Command.Right = true;
                break;
            case GLFW_KEY_E:
                Command.Fly = true;
                break;
            case GLFW_KEY_Q:
                Command.Fall = true;
                break;
            case GLFW_KEY_SPACE:
                Command.Jump = true;
                break;
            case GLFW_KEY_LEFT_CONTROL:
                Command.Squat = true;
                break;
            case GLFW_KEY_LEFT_ALT: {
                std::shared_ptr<WindowSystem> window_system = GWindowSystem;
                window_system->SetFocusMode(!window_system->GetFocusMode());
            }
                break;
            case GLFW_KEY_LEFT_SHIFT:
                Command.Sprint = true;
                break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE:
                break;
            case GLFW_KEY_A:
                Command.Left = false;
                break;
            case GLFW_KEY_S:
                Command.Backward = false;
                break;
            case GLFW_KEY_W:
                Command.Forward = false;
                break;
            case GLFW_KEY_D:
                Command.Right = false;
                break;
            case GLFW_KEY_E:
                Command.Fly = false;
                break;
            case GLFW_KEY_Q:
                Command.Fall = false;
                break;
            case GLFW_KEY_LEFT_CONTROL:
                Command.Squat = false;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                Command.Sprint = false;
                break;
        }
    }
}

void InputSystem::CalculateCursorDeltaAngles()
{
    std::array<int, 2> window_size = GWindowSystem->GetWindowSize();

    if (window_size[0] < 1 || window_size[1] < 1)
        return;

    std::shared_ptr<RenderCamera> render_camera = GRenderSystem->GetRenderCamera();
    Vector2f                      fov           = render_camera->GetFov();

    Vector2f cursor_delta = {DegreesToRadians(CursorDeltaX), DegreesToRadians(CursorDeltaY)};

    CursorDeltaYaw = cursor_delta.x() / window_size[0] * fov.x();
    CursorDeltaPitch = -cursor_delta.y() / window_size[1] * fov.y();
}
