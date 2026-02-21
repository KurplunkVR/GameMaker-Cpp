#pragma once

struct InputState {
    bool keyDown[256];
    int mouseX, mouseY;
    bool mouseButtons[8];
    // Add gamepad, touch, etc.
};

class IInput {
public:
    virtual ~IInput() {}
    virtual void PollEvents() = 0;
    virtual InputState GetState() = 0;
};
