#pragma once
#include <cstddef>

class IStream {
  public:
    template<class T>
    T read() {
        T t;
        read((std::byte*)&t, sizeof(T));
        return t;
    }

    virtual int read(std::byte* data, int len) = 0;
};
