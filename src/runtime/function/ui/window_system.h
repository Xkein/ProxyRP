#pragma once

#include "platform/string.h"

#include <functional>

class GLFWwindow;

struct WindowCreateInfo
{
    int    Width;
    int    Height;
    String Title;
    bool   IsFullScreen;
};

class WindowSystem
{
public:
    WindowSystem() = default;
    ~WindowSystem();

    void Initialize(WindowCreateInfo create_info);

    void PollEvents() const;

    bool ShouldClose() const;

    void SetTitle(const String& title);

    GLFWwindow*        GetWindow() const { return Window; }
    std::array<int, 2> GetWindowSize() const;

    bool IsMouseButtonDown(int button) const;

    bool GetFocusMode() const
    {
        return IsFocusMode;
    }
    void SetFocusMode(bool mode);
    
    typedef std::function<void()>                   OnResetFunc;
    typedef std::function<void(int, int, int, int)> OnKeyFunc;
    typedef std::function<void(unsigned int)>       OnCharFunc;
    typedef std::function<void(int, unsigned int)>  OnCharModsFunc;
    typedef std::function<void(int, int, int)>      OnMouseButtonFunc;
    typedef std::function<void(double, double)>     OnCursorPosFunc;
    typedef std::function<void(int)>                OnCursorEnterFunc;
    typedef std::function<void(double, double)>     OnScrollFunc;
    typedef std::function<void(int, const char**)>  OnDropFunc;
    typedef std::function<void(int, int)>           OnWindowSizeFunc;
    typedef std::function<void()>                   OnWindowCloseFunc;
    typedef std::function<void(int, int)>           OnFramebufferSizeFunc;

    void RegisterOnResetFunc(OnResetFunc func) { OnResetFuncList.push_back(func); }
    void RegisterOnKeyFunc(OnKeyFunc func) { OnKeyFuncList.push_back(func); }
    void RegisterOnCharFunc(OnCharFunc func) { OnCharFuncList.push_back(func); }
    void RegisterOnCharModsFunc(OnCharModsFunc func) { OnCharModsFuncList.push_back(func); }
    void RegisterOnMouseButtonFunc(OnMouseButtonFunc func) { OnMouseButtonFuncList.push_back(func); }
    void RegisterOnCursorPosFunc(OnCursorPosFunc func) { OnCursorPosFuncList.push_back(func); }
    void RegisterOnCursorEnterFunc(OnCursorEnterFunc func) { OnCursorEnterFuncList.push_back(func); }
    void RegisterOnScrollFunc(OnScrollFunc func) { OnScrollFuncList.push_back(func); }
    void RegisterOnDropFunc(OnDropFunc func) { OnDropFuncList.push_back(func); }
    void RegisterOnWindowSizeFunc(OnWindowSizeFunc func) { OnWindowSizeFuncList.push_back(func); }
    void RegisterOnWindowCloseFunc(OnWindowCloseFunc func) { OnWindowCloseFuncList.push_back(func); }

private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void CharCallback(GLFWwindow* window, unsigned int codepoint);
    static void CharModsCallback(GLFWwindow* window, unsigned int codepoint, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void CursorEnterCallback(GLFWwindow* window, int entered);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void DropCallback(GLFWwindow* window, int count, const char** paths);
    static void WindowSizeCallback(GLFWwindow* window, int width, int height);
    static void WindowCloseCallback(GLFWwindow* window);
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    void OnReset();
    void OnKey(int key, int scancode, int action, int mods);
    void OnChar(unsigned int codepoint);
    void OnCharMods(int codepoint, unsigned int mods);
    void OnMouseButton(int button, int action, int mods);
    void OnCursorPos(double xpos, double ypos);
    void OnCursorEnter(int entered);
    void OnScroll(double xoffset, double yoffset);
    void OnDrop(int count, const char** paths);
    void OnWindowClose();
    void OnWindowSize(int width, int height);
    void OnFramebufferSize(int width, int height);

private:
    GLFWwindow* Window {nullptr};
    int         Width {0};
    int         Height {0};

    bool IsFocusMode {false};
    bool Resized {false};

    std::vector<OnResetFunc>           OnResetFuncList;
    std::vector<OnKeyFunc>             OnKeyFuncList;
    std::vector<OnCharFunc>            OnCharFuncList;
    std::vector<OnCharModsFunc>        OnCharModsFuncList;
    std::vector<OnMouseButtonFunc>     OnMouseButtonFuncList;
    std::vector<OnCursorPosFunc>       OnCursorPosFuncList;
    std::vector<OnCursorEnterFunc>     OnCursorEnterFuncList;
    std::vector<OnScrollFunc>          OnScrollFuncList;
    std::vector<OnDropFunc>            OnDropFuncList;
    std::vector<OnWindowSizeFunc>      OnWindowSizeFuncList;
    std::vector<OnWindowCloseFunc>     OnWindowCloseFuncList;
    std::vector<OnFramebufferSizeFunc> OnFramebufferSizeFuncList;
};
