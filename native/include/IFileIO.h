#pragma once
#include <cstddef>

class IFileIO {
public:
    virtual ~IFileIO() {}
    virtual bool LoadFile(const char* path, void*& data, size_t& size) = 0;
    virtual bool SaveFile(const char* path, const void* data, size_t size) = 0;
    // Add async, asset-pack, etc.
};
