#pragma once
#include <cstddef>
#include <chrono>

class IStream {
  public:
    template<class T>
    T read(std::chrono::milliseconds timeout) {
        T t;
        read((std::byte*)&t, sizeof(T), timeout);
        return t;
    }

    virtual int read(std::byte* data, int len, std::chrono::milliseconds timeout) = 0;
};
