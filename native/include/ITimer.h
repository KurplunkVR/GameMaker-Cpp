#pragma once
#include <cstdint>

class ITimer {
public:
    virtual ~ITimer() {}
    virtual void Sleep(int ms) = 0;
    virtual uint64_t GetTicks() = 0;
};
