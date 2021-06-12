#pragma once
#include <cstdint>

namespace Hlw8112N {

template<class T>
struct RegMeta {
    T initial = T();
    uint8_t addr;
    uint8_t size = sizeof(T);
    bool readonly = false;
};

template<class T>
struct RegTraits {

};
}