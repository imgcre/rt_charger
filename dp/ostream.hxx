#pragma once
#include <cstddef>

class OStream {
  public:
    template<class T>
    void write(const T& t) {
        write((std::byte*)(void*)&t, sizeof(T));
    }

    virtual void write(std::byte* data, int len) = 0;
};
