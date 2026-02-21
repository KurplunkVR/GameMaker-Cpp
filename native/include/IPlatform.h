#pragma once
#include "IRenderer.h"
#include "IAudio.h"
#include "IInput.h"
#include "IFileIO.h"
#include "ITimer.h"

class IPlatform {
public:
    virtual ~IPlatform() {}
    virtual IRenderer* GetRenderer() = 0;
    virtual IAudio* GetAudio() = 0;
    virtual IInput* GetInput() = 0;
    virtual IFileIO* GetFileIO() = 0;
    virtual ITimer* GetTimer() = 0;
};
