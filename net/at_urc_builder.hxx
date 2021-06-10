#pragma once
#include <vector>
#include <concepts>
#include <rtthread.h>
#include <at.h>
#include "at_urc_builder_base.hxx"
#include "at_comp_base.hxx"
#include <string_view>
#include <stdexcept>
#include <cstddef>
#include <functional>
#include <string.hxx>
#include <tuple>
#include <utilities/limited_slots.hxx>
#include <os/event_set.hxx>
#include <optional>
#include <utilities/offset.hxx>
#include <type_traits>
#include <utilities/mp.hxx>

template<class T>
struct AtUrcBuilderTraits {
    using DefaultSlots = std::nullptr_t;
};

template<class T, class Comp> class AtUrcBuilder;

template<class T>
struct EventEmitter {
    template<class U, class Comp> friend class AtUrcBuilder;
  private:
    EventEmitter(std::vector<int>& e): e(e) { }

  public:
    auto& operator<<(T t) {
        e.push_back((int)t);
        return *this;
    }

  private:
    std::vector<int>& e;
};

template<class T>
concept EnumOrIntergral = std::integral<T> || std::is_enum_v<T>;

template<class T, class Comp>
class AtUrcBuilder: public AtUrcBuilderBase {
    friend Comp;
  public:
    template<class... Args>
    using MemberFunc = void(T::*)(Args...);
    using Traits = AtUrcBuilderTraits<T>;

    AtUrcBuilder(Comp* comp): comp(comp) { }

    template<StringLiteral Prefix, StringLiteral Suffix = kEndl>
    auto& add(MemberFunc<> func) {
        static auto f = func;
        AtUrcBuilderBase::add(Prefix.value, Suffix.value, [](at_client_t client, const char* data, rt_size_t size){
            run([&](auto inst){
                ((*inst).*f)();
            }, client);
        });
        return *this;
    }

    template<StringLiteral Prefix, StringLiteral Suffix = kEndl>
    auto& add(MemberFunc<std::string_view> func) {
        static auto f = func;
        AtUrcBuilderBase::add(Prefix.value, Suffix.value, [](at_client_t client, const char* data, rt_size_t size){
            run([&](auto inst){
                ((*inst).*f)({data, size});
            }, client);
        });
        return *this;
    }

    template<StringLiteral Prefix, StringLiteral Suffix = kEndl>
    auto& add(MemberFunc<std::string_view, std::string_view, std::string_view> func) {
        static auto f = func;
        AtUrcBuilderBase::add(Prefix.value, Suffix.value, [](at_client_t client, const char* data, rt_size_t size){
            run([&](auto inst){
                ((*inst).*f)({data, size}, Prefix.view(), Suffix.view());
            }, client);
        });
        return *this;
    }

    struct Options {
        enum Values {
            UnwrapQuota = 1 << 0,
            RemoveLeadingPad = 1 << 1,
        };
    };
    
  public:
    auto& set(typename Traits::DefaultSlots& slots) {
        defaultSlotsOffset = Offset(*(T*)comp, slots);
        return *this;
    }

    auto& set(EventSet& event) {
        defaultEventOffset = Offset(*(T*)comp, event);
        return *this;
    }

    auto& set(typename Traits::DefaultSlots& slots, EventSet& event) {
        set(slots);
        set(event);
        return *this;
    }

    // template<class... Args>
    // auto& setDefault(Args&&... args) requires(sizeof...(args) >= 2) {
    //     std::make_tuple(set(args)...);
    //     return *this;
    // }

    template<class R, class... Args>
    using ProbedMemberFunc = R(T::*)(Args...);

    template<class R, class... Args>
    using ProbedLambdaFunc = std::function<R(Args...)>;

    template<class F> struct IsProbedMemberFunc: public std::false_type { };
    template<class R, class... Args> struct IsProbedMemberFunc<ProbedMemberFunc<R, Args...>>: public std::true_type { };

    template<class F> auto wrapUp(F func) { 
        if constexpr(IsProbedMemberFunc<F>::value) {
            return func; 
        } else {
            return std::function{func};
        }
        
    }

    static constexpr auto kDefaultOpt = (typename Options::Values)(Options::UnwrapQuota | Options::RemoveLeadingPad);

    template<StringLiteral Prefix, StringLiteral Suffix, class F>
    auto& probe(F func, typename Options::Values opt = kDefaultOpt) {
        probeInternal<Prefix, Suffix>(wrapUp(func), defaultSlotsOffset, defaultEventOffset, std::nullopt, opt);
        return *this;
    }

    template<StringLiteral Prefix, StringLiteral Suffix, class F>
    auto& probe(F func, int offset, typename Options::Values opt = kDefaultOpt) {
        probeInternal<Prefix, Suffix>(wrapUp(func), defaultSlotsOffset, defaultEventOffset, offset, opt);
        return *this;
    }

    template<StringLiteral Prefix, StringLiteral Suffix, class Slots, class F>
    auto& probe(F func, Slots& slots, typename Options::Values opt = kDefaultOpt) {
        probeInternal<Prefix, Suffix>(wrapUp(func), std::optional{Offset(*(T*)comp, slots)}, defaultEventOffset, std::nullopt, opt);
        return *this;
    }

    template<StringLiteral Prefix, StringLiteral Suffix, class Slots, class F>
    auto& probe(F func, Slots& slots, int offset, typename Options::Values opt = kDefaultOpt) {
        probeInternal<Prefix, Suffix>(wrapUp(func), std::optional{Offset(*(T*)comp, slots)}, defaultEventOffset, offset, opt);
        return *this;
    }

    template<StringLiteral Prefix, StringLiteral Suffix, class Slots, class F>
    auto& probe(F func, Slots& slots, EventSet& event, int offset, typename Options::Values opt = kDefaultOpt) {
        probeInternal<Prefix, Suffix>(wrapUp(func), std::optional{Offset(*(T*)comp, slots)}, std::optional{Offset(*(T*)comp, event)}, offset, opt);
        return *this;
    }

  private:
    template<class Slots> requires(SlotsInfo<Slots>::value)
    struct ArgMapper {
        ArgMapper(std::shared_ptr<T> inst, std::optional<Offset<T, Slots>> slotOffset): inst(inst), slotOffset(slotOffset) { }

        template<class X>
        std::tuple<X, bool> operator()(std::string_view a) {
            if constexpr(std::same_as<typename SlotsInfo<Slots>::Element, X>) {
                slotId = parse<int>(a);
                if(!slotOffset)
                    throw std::runtime_error{"slots not set"};
                return {slotOffset->recover(*inst).acuqire(*slotId), true};
            } else if constexpr(SpecOf<X, EventEmitter>) {
                return {X(events), false};
            } else {
                return {parse<X>(a), true};
            }
        }

        std::optional<int> getSlotId() {
            return slotId;
        }

        std::vector<int> getEvents() {
            return move(events);
        }

      private:
        std::shared_ptr<T> inst;
        std::optional<Offset<T, Slots>> slotOffset;
        std::optional<int> slotId;
        std::vector<int> events = {};
    };

    template<StringLiteral Prefix, StringLiteral Suffix, class Slots, class F>
    void probeInternal(
      F func, 
      std::optional<Offset<T, Slots>> slotsOffset, 
      std::optional<Offset<T, EventSet>> eventOffset, 
      std::optional<int> offset, 
      typename Options::Values opt) {
        static auto f = func;
        static auto o = (int)opt;
        static auto oSlots = slotsOffset;
        static auto oEvent = eventOffset;
        static auto ost = offset;
        AtUrcBuilderBase::add(Prefix.value, Suffix.value, [](at_client_t client, const char* data, rt_size_t size) {
            using Mapper = ArgMapper<Slots>;
            runProbe<Prefix, Suffix, Mapper>([&](auto inst) {
                return Mapper{inst, oSlots};
            }, f, client, data, size, o, [&](auto inst, auto m, auto result) {
                int bits = 0;
                if(ost) result.push_back(*ost);
                auto mEvents = m.getEvents();
                auto slotId = 0;
                if(m.getSlotId()) slotId = *m.getSlotId();
                result.insert(result.end(), mEvents.begin(), mEvents.end()); //TODO: 重构 使用range
                for(const auto r: result)
                    bits |= 1 << (r + slotId);
                if(bits == 0) return;
                if(!oEvent) throw std::runtime_error{"event not set"};
                oEvent->recover(*inst).set(bits);
            });
        });
    }

    static void run(std::function<void(std::shared_ptr<T>)> cb, at_client_t client) {
        auto inst = T::fromUrc(client);
        if(inst == nullptr) return;
        try {
            cb(inst);
        } catch(std::exception& e) {
            rt_kprintf("E: AT URC HANDING(%s)\n", e.what());
        } catch(...) {
            rt_kprintf("E: AT URC HANDING\n");
        }
    }

    template<class B, class... Args>
    static std::vector<int> invoke(B bound, Args&&... args) {
        using R = decltype(std::invoke(bound, args...));
        if constexpr(std::is_void_v<R>) {
            std::invoke(bound, args...);
            return {};
        } else {
            auto r = std::invoke(bound, args...);
            if constexpr(EnumOrIntergral<R>) {
                return {r};
            } else if constexpr(SpecOf<R, std::optional>) {
                auto result = std::vector<int>{};
                if(r) result.push_back(*r);
                return result;
            } else if constexpr(SpecOf<R, std::vector>) {
                return r;
            }
        }
    }

    template<class M>
    struct ArgProvider {
        ArgProvider(M& m, std::vector<std::string> argStrs, int opt): m(m), argStrs(std::move(argStrs)), opt(opt) { }
        template<class Arg>
        Arg operator()() {
            auto&& [r, next] = m.template operator()<Arg>(PreprocessArg(std::string_view{argStrs[i]}, opt));
            if(next) i++;
            return r;
        }
        M& m;
        std::vector<std::string> argStrs;
        int opt;
        int i = 0;
    };

    template<class R, class P, class... Args>
    static std::vector<int> invokeUrcCallback(ProbedMemberFunc<R, Args...> f, std::shared_ptr<T> t, P& p) {
        return invoke(std::bind_front(f, t.get()), p.template operator()<Args>()...);
    }

    template<class R, class P, class... Args>
    static std::vector<int> invokeUrcCallback(ProbedLambdaFunc<R, Args...> f, std::shared_ptr<T> t, P& p) {
        return invoke(f, p.template operator()<Args>()...);
    }

    template<class F> struct ProbedArgCount;
    template<class R, class... Args> struct ProbedArgCount<ProbedMemberFunc<R, Args...>>: public std::integral_constant<int, sizeof...(Args)> {};
    template<class R, class... Args> struct ProbedArgCount<ProbedLambdaFunc<R, Args...>>: public std::integral_constant<int, sizeof...(Args)> {};

    template<StringLiteral Prefix, StringLiteral Suffix, class M, class F>
    static void runProbe(std::function<M(std::shared_ptr<T>)> maker, F f, at_client_t client, const char* data, rt_size_t size, int opt, std::function<void(std::shared_ptr<T>, M&, std::vector<int>)> then = {}) {
        run([&](auto inst){
            auto m = maker(inst);
            auto p = ArgProvider{m, getArgs<Prefix, Suffix>(data, size, ProbedArgCount<F>::value), opt};
            auto result = invokeUrcCallback(f, inst, p);
            if(then) then(inst, m, std::move(result));
        }, client);
    }

    template<StringLiteral Prefix, StringLiteral Suffix>
    static std::vector<std::string> getArgs(const char* data, rt_size_t size, size_t cnt) {
        auto argStrView = std::string_view{data, size}.substr(Prefix.view().size(), size - Prefix.view().size() - Suffix.view().size());
        auto argStrs = split(argStrView, ',', cnt - 1);
        return argStrs;
    }

    static std::string_view PreprocessArg(std::string_view sv, int opt) {
        if(opt & Options::RemoveLeadingPad) {
            int noPadPos = 0;
            for(auto i = 0u; i < sv.size(); i++) {
                if(sv[i] != ' ') break;
                noPadPos++;
            }
            sv = sv.substr(noPadPos, sv.size() - noPadPos);
        }
        if(opt & Options::UnwrapQuota) {
            if(sv[0] == '"' && sv[sv.size() - 1] == '"') {
                sv = sv.substr(1, sv.size() - 2);
            }
        }
        return sv;
    }

    std::vector<at_urc> getResult() {
        return AtUrcBuilderBase::getResult();
    }
  private:
    Comp* comp;
    static std::optional<Offset<T, typename Traits::DefaultSlots>> defaultSlotsOffset;
    static std::optional<Offset<T, EventSet>> defaultEventOffset;
    
};

template<class T, class Comp>
std::optional<Offset<T, typename AtUrcBuilder<T, Comp>::Traits::DefaultSlots>> AtUrcBuilder<T, Comp>::defaultSlotsOffset = {};

template<class T, class Comp>
std::optional<Offset<T, EventSet>> AtUrcBuilder<T, Comp>::defaultEventOffset = {};
