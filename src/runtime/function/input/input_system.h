#pragma once

struct GameCommand
{
    operator bool() const
    {
        return !Invalid;
    }
    // W
    bool Forward : 1;
    // S
    bool Backward : 1;
    // A
    bool Left : 1;
    // D
    bool Right : 1;
    // Space
    bool Jump : 1;
    // Left Ctrl
    bool Squat : 1;
    // Left Shift
    bool Sprint : 1;
    // E
    bool Fly : 1;
    // Q
    bool Fall : 1;

    bool Invalid : 1;
};

class InputSystem
{

public:
    void OnKey(int key, int scancode, int action, int mods);
    void OnCursorPos(double x, double y);

    void Initialize();
    void Clear();

    void Tick();

    void ResetGameCommand() {
        Command = {};
    }
    GameCommand GetGameCommand() const {
        return Command;
    }

    int CursorDeltaX {0};
    int CursorDeltaY {0};

    float CursorDeltaYaw {0};
    float CursorDeltaPitch {0};

private:
    void OnKeyInGameMode(int key, int scancode, int action, int mods);
    void CalculateCursorDeltaAngles();

    GameCommand Command {};

    int LastCursorX {0};
    int LastCursorY {0};
};

