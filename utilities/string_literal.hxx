#include <algorithm>
#include <string_view>

template<size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    std::string_view view() const {
        return {value, N - 1};
    }

    char value[N];
    size_t length = N;
};