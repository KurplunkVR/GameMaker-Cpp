#pragma once

class IAudio {
public:
    virtual ~IAudio() {}
    virtual bool Init() = 0;
    virtual void PlaySound(const char* path) = 0;
    virtual void StopAll() = 0;
    // Add more: stream, setVolume, etc.
};
