#pragma once
#include <memory>

template<class T>
class Singleton {
public:
    using singleton_t = Singleton<T>;
    template<class... A>
    static std::shared_ptr<T> get(A&&... a) {
        static auto inst = std::shared_ptr<T>(new T(std::forward<A>(a)...));
        return inst;
    }
};
