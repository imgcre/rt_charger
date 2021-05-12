#pragma once

#include <functional>
#include <vector>
#include <utility>
#include <tuple>
#include <memory>
#include <variant>
#include <optional>
#include <rtthread.h>
#include "post.hxx"
#include "deliver.hxx"

template<class T>
struct Signals;

template <class L, class F>
class Deliver;

//如果是DeliverProxy, 则不返回，转而调用onReturn, onReturn是function<void(R)>
template<class R, class ...P>
struct Signals<R(P...)> {
    using result_t = R;
    using params_t = std::tuple<P...>;
    template<int N>
    using param_t = std::tuple_element_t<N, params_t>;
    using cb_f = R(P...);
    using ret_f = void(std::variant<R, std::exception_ptr>);
    using ret_sig_t = Signals<ret_f>;
    using signal_f = void(ret_sig_t, P...);

    void operator+=(std::function<signal_f> cb) {
        cbs.reserve(cbs.size() + 1);
        cbs.push_back(cb);
    }

    void operator+=(std::function<cb_f> cb) {
        (*this) += [=](ret_sig_t r, P ...p) {
            try {
                auto result = cb(p...);
                r(result);
            } catch(const std::exception& e) {
                r(std::current_exception());
            }


        };
    }

    template<class L, class F>
    void operator +=(std::shared_ptr<Deliver<L, F>> other) {
        other->addTo(*this);
    }

    void operator()(ret_sig_t r, P ...p) {
        for(const auto& cb: cbs) {
            cb(r, p...);
        }
    }

    void operator()(std::function<ret_f> r, P ...p) {
        for(const auto& cb: cbs) {
            auto signal = ret_sig_t();
            signal += r;
            cb(signal, p...);

        }
    }

    template<class L, class F>
    void operator()(std::shared_ptr<Deliver<L, F>>&& r, P ...p) {
        for(const auto& cb: cbs) {
            auto signal = ret_sig_t();
            r->addTo(signal, DeliverType::ShortTerm);
            cb(signal, p...);
        }
    }

    template<class L, class F>
    void operator()(const std::shared_ptr<Deliver<L, F>>& r, P ...p) {
        for(const auto& cb: cbs) {
            auto signal = ret_sig_t();
            r->addTo(signal);
            cb(signal, p...);
        }
    }

    void operator()(P ...p) {
        for(const auto& cb: cbs) {
            cb(ret_sig_t(), p...);
        }
    }

    bool empty() {
        return cbs.empty();
    }

    std::vector<std::function<signal_f>> cbs;
};


template<class ...P>
struct Signals<void(P...)> {
    using result_t = void;
    using params_t = std::tuple<P...>;
    template<int N>
    using param_t = std::tuple_element_t<N, params_t>;
    using cb_f = void(P...);
    using ret_f = void(std::optional<std::exception_ptr>);
    using ret_sig_t = Signals<ret_f>;
    using signal_f = void(ret_sig_t, P...);

    void operator+=(std::function<signal_f> cb) {
        cbs.reserve(cbs.size() + 1);
        cbs.push_back(cb);
    }

    void operator+=(std::function<cb_f> cb) {
        (*this) += [=](ret_sig_t r, P ...p) {
            try {
                cb(p...);
                r({});
            } catch(const std::exception& e) {
                r({std::current_exception()});
            }
        };
    }

    template<class L, class F>
    void operator +=(std::shared_ptr<Deliver<L, F>> other) {
        other->addTo(*this);
    }

    void operator()(P ...p) const {
        for(const auto& cb: cbs) {
            cb(p...);
        }
    }

    void operator()(std::function<ret_f> r, P ...p) {
        for(const auto& cb: cbs) {
            auto signal = ret_sig_t();
            signal += r;
            cb(signal, p...);
        }
    }

    template<class L, class F>
    void operator()(std::shared_ptr<Deliver<L, F>>&& r, P ...p) {
        for(const auto& cb: cbs) {
            auto signal = ret_sig_t();
            r->addTo(signal, DeliverType::ShortTerm);
            cb(signal, p...);
        }
    }

    template<class L, class F>
    void operator()(const std::shared_ptr<Deliver<L, F>>& r, P ...p) {
        for(const auto& cb: cbs) {
            auto signal = ret_sig_t();
            r->addTo(signal);
            cb(signal, p...);
        }
    }

    void operator()(P ...p) {
        for(const auto& cb: cbs) {
            cb(ret_sig_t(), p...);
        }
    }

    bool empty() {
        return cbs.empty();
    }

    std::vector<std::function<signal_f>> cbs;
};

template<class L, class F>
void runOn(std::shared_ptr<Deliver<L, F>>&& r) {
    auto signal = Signals<void()>{};
    r->addTo(signal, DeliverType::ShortTerm);
    signal();
}

template<class F, class T>
auto signal(F&& f, T&& t) {
    return [f = std::forward<F>(f), t = std::forward<T>(t)](auto&&... args) {
        return (t->*f)(std::forward<decltype(args)>(args)...);
    };
}
