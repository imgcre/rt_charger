#pragma once
#include <memory>

template<class T, class U = T>
class Singleton {
public:
    using singleton_t = Singleton<T, U>;
    template<class... A>
    static std::shared_ptr<U> get(A&&... a) {
        static auto inst = std::shared_ptr<T>(new T(std::forward<A>(a)...));
        return inst;
    }
};
