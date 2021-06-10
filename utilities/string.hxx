#pragma once
#include <vector>
#include <string_view>
#include <string>
#include <concepts>
#include <charconv>

std::vector<std::string> split(std::string_view str, char splitter, int cnt = -1);

template<class T> requires(std::integral<T> || std::floating_point<T>)
T parse(std::string_view sv) {
    T t;
    std::from_chars(sv.data(), sv.data() + sv.size(), t);
    return t;
}

template<std::same_as<std::string_view> T>
T parse(std::string_view sv) {
    return sv;
}