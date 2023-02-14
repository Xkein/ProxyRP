#include "window_system.h"
#include "core/log/log_system.h"

#include <GLFW/glfw3.h>
#include <array>

WindowSystem* GetWindowSystem(GLFWwindow* window) {
    return (WindowSystem*)glfwGetWindowUserPointer(window);
}

#define WINDOW_SYSTEM_OPERATION(operation) { \
    WindowSystem* window_system = GetWindowSystem(window); \
    if (window_system) { window_system->operation; } }

#define WINDOW_SYSTEM_OPERATION2(operation1, operation2) { \
    WindowSystem* window_system = GetWindowSystem(window); \
    if (window_system) { window_system->operation1; window_system->operation2; } }


WindowSystem::~WindowSystem()
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}

void WindowSystem::Initialize(WindowCreateInfo create_info)
{
    if (!glfwInit())
    {
        LOG_FATAL("failed to initialize GLFW");
    }

    Width  = create_info.Width;
    Height = create_info.Height;
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    Window = glfwCreateWindow(Width, Height, create_info.Title.c_str(), nullptr, nullptr);
    if (!Window)
    {
        LOG_FATAL("failed to create window");
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer(Window, this);

    glfwSetKeyCallback(Window, KeyCallback);
    glfwSetCharCallback(Window, CharCallback);
    glfwSetCharModsCallback(Window, CharModsCallback);
    glfwSetMouseButtonCallback(Window, MouseButtonCallback);
    glfwSetCursorPosCallback(Window, CursorPosCallback);
    glfwSetCursorEnterCallback(Window, CursorEnterCallback);
    glfwSetScrollCallback(Window, ScrollCallback);
    glfwSetDropCallback(Window, DropCallback);
    glfwSetWindowSizeCallback(Window, WindowSizeCallback);
    glfwSetWindowCloseCallback(Window, WindowCloseCallback);
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    glfwSetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void WindowSystem::PollEvents() const
{
    glfwPollEvents();
}

bool WindowSystem::ShouldClose() const
{
    return glfwWindowShouldClose(Window);
}

void WindowSystem::SetTitle(const String& title)
{
    glfwSetWindowTitle(Window, title.c_str());
}

std::array<int, 2> WindowSystem::GetWindowSize() const
{
    return {Width, Height};
}

bool WindowSystem::IsMouseButtonDown(int button) const
{
    if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
    {
        return false;
    }
    return glfwGetMouseButton(Window, button) == GLFW_PRESS;
}

void WindowSystem::SetFocusMode(bool mode)
{
    IsFocusMode = mode;
    glfwSetInputMode(Window, GLFW_CURSOR, IsFocusMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void WindowSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    WINDOW_SYSTEM_OPERATION(OnKey(key, scancode, action, mods));
}
void WindowSystem::CharCallback(GLFWwindow* window, unsigned int codepoint)
{
    WINDOW_SYSTEM_OPERATION(OnChar(codepoint));
}
void WindowSystem::CharModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
{
    WINDOW_SYSTEM_OPERATION(OnCharMods(codepoint, mods));
}
void WindowSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    WINDOW_SYSTEM_OPERATION(OnMouseButton(button, action, mods));
}
void WindowSystem::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    WINDOW_SYSTEM_OPERATION(OnCursorPos(xpos, ypos));
}
void WindowSystem::CursorEnterCallback(GLFWwindow* window, int entered)
{
    WINDOW_SYSTEM_OPERATION(OnCursorEnter(entered));
}
void WindowSystem::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    WINDOW_SYSTEM_OPERATION(OnScroll(xoffset, yoffset));
}
void WindowSystem::DropCallback(GLFWwindow* window, int count, const char** paths)
{
    WINDOW_SYSTEM_OPERATION(OnDrop(count, paths));
}
void WindowSystem::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    WINDOW_SYSTEM_OPERATION(OnWindowSize(width, height));
    WINDOW_SYSTEM_OPERATION2(Width = width, Height = height);
}
void WindowSystem::WindowCloseCallback(GLFWwindow* window)
{
    WINDOW_SYSTEM_OPERATION(OnWindowClose());
    glfwSetWindowShouldClose(window, true);
}
void WindowSystem::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    WINDOW_SYSTEM_OPERATION(OnFramebufferSize(width, height));
}

void WindowSystem::OnReset()
{
    for (auto& func : OnResetFuncList)
        func();
}
void WindowSystem::OnKey(int key, int scancode, int action, int mods)
{
    for (auto& func : OnKeyFuncList)
        func(key, scancode, action, mods);
}
void WindowSystem::OnChar(unsigned int codepoint)
{
    for (auto& func : OnCharFuncList)
        func(codepoint);
}
void WindowSystem::OnCharMods(int codepoint, unsigned int mods)
{
    for (auto& func : OnCharModsFuncList)
        func(codepoint, mods);
}
void WindowSystem::OnMouseButton(int button, int action, int mods)
{
    for (auto& func : OnMouseButtonFuncList)
        func(button, action, mods);
}
void WindowSystem::OnCursorPos(double xpos, double ypos)
{
    for (auto& func : OnCursorPosFuncList)
        func(xpos, ypos);
}
void WindowSystem::OnCursorEnter(int entered)
{
    for (auto& func : OnCursorEnterFuncList)
        func(entered);
}
void WindowSystem::OnScroll(double xoffset, double yoffset)
{
    for (auto& func : OnScrollFuncList)
        func(xoffset, yoffset);
}
void WindowSystem::OnDrop(int count, const char** paths)
{
    for (auto& func : OnDropFuncList)
        func(count, paths);
}
void WindowSystem::OnWindowClose()
{
    for (auto& func : OnWindowCloseFuncList)
        func();
}
void WindowSystem::OnWindowSize(int width, int height)
{
    for (auto& func : OnWindowSizeFuncList)
        func(width, height);
}
void WindowSystem::OnFramebufferSize(int width, int height)
{
    for (auto& func : OnWindowSizeFuncList)
        func(width, height);
}