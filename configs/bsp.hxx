#pragma once
#include <type_traits>

namespace Config {
class Bsp {
public:
    static constexpr int kPortNum = 2;

    template<class T, int R>
    using assert_t = std::enable_if_t<R >= 0 && R < kPortNum, T>;
};
}