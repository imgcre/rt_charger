#pragma once
#include <rtthread.h>

enum class Priority {
    High, Middle, Low,
};

template<Priority Proi>
struct PriorityTrait {
    //static constexpr rt_uint8_t value = 0;
};

template<>
struct PriorityTrait<Priority::High> {
    static constexpr rt_uint8_t value = 2;
};

template<>
struct PriorityTrait<Priority::Middle> {
    static constexpr rt_uint8_t value = 8;
};

template<>
struct PriorityTrait<Priority::Low> {
    static constexpr rt_uint8_t value = 10;
};
