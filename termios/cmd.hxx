#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <deque>
#include <type_traits>
#include <functional>
#include <concepts>
#include <hal/port.hxx>
#include <stdexcept>

class Cmd {
public:
    Cmd(int argc, char** argv);
    void operator()(std::function<void(Cmd& cmd)> cb);

public:
    template<class T>
    auto get(const char* name = nullptr) -> std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>, T> {
        return (T)atoi(getFrom(name));
    }

    template<std::same_as<Port> T>
    auto get(const char* name = nullptr) {
        auto result = AbsPort(atoi(getFrom(name)));
        if(!result.validate()) {
            throw std::runtime_error{"port out of range"};
        }
        return result;
    }

    template<class T>
    auto get(const char* name = nullptr) -> std::enable_if_t<std::is_floating_point_v<T>, T> {
        return (T)atof(getFrom(name));
    }

    template<class T>
    auto get(const char* name = nullptr) -> std::enable_if_t<std::is_same_v<char*, T>, T> {
        return getFrom(name);
    }

    template<class T>
    auto get(const char* name = nullptr) -> std::enable_if_t<std::is_same_v<std::string, T>, T> {
        return getFrom(name);
    }

    template<class T = std::string>
    auto select(std::unordered_map<T, std::function<void()>>&& sel) {
        select(nullptr, std::move(sel));
    }

    template<class T>
    auto select(const char* name, std::unordered_map<T, std::function<void()>>&& sel) {
        auto key = get<T>(name);
        auto found = sel.find(key);
        assertV(found != sel.end(), [&](){
            auto valid = std::string{};
            auto size = sel.size();
            auto count = 0u;
            for(const auto& kvp: sel) {
                valid += getStr(kvp.first);
                if(count + 1 < size) {
                    valid += "|";
                }
                count++;
            }
            return "bad arg " + key + ", valid are: [" + valid + "]";
        });
        found->second();
    }

    void assertV(bool predict, std::string msg);
    void assertV(bool predict, std::function<std::string()> gen);

private:
    char* getFromPos();
    char* getFromName(const char* name);
    char* getFrom(const char* name);

    template<class T>
    auto getStr(T& t) -> std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, std::string> {
        return t;
    }

    template<class T>
    auto getStr(T& t) -> std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>, std::string> {
        return to_string(t);
    }

private:
    std::deque<char*> q;
    std::unordered_map<std::string, char*> m;
};