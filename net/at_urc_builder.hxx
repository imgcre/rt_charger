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

template<class T>
struct AtUrcBuilderTraits {
    using DefaultSlots = std::nullptr_t;
};

template<class T, class Comp> class AtUrcBuilder;

template<class T>
struct EventBuilder {
    EventBuilder(): e(*(std::vector<int>*)nullptr) {

    }

    EventBuilder(std::vector<int>& e): e(e) {

    }

    template<class U, class Comp> friend class AtUrcBuilder;
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

    AtUrcBuilder(Comp* comp): comp(comp) {

    }

    using NoArgFunc = MemberFunc<>;
    template<StringLiteral Prefix, StringLiteral Suffix = kEndl>
    auto& add(NoArgFunc func) requires std::derived_from<T, AtCompBase> {
        static NoArgFunc f = func;
        AtUrcBuilderBase::add(Prefix.value, Suffix.value, [](at_client_t client, const char* data, rt_size_t size){
            run([&](auto inst){
                ((*inst).*f)();
            }, client);
        });
        return *this;
    }

    using SimpleFunc = MemberFunc<std::string_view>;
    template<StringLiteral Prefix, StringLiteral Suffix = kEndl>
    auto& add(SimpleFunc func) requires std::derived_from<T, AtCompBase> {
        static auto f = func;
        AtUrcBuilderBase::add(Prefix.value, Suffix.value, [](at_client_t client, const char* data, rt_size_t size){
            run([&](auto inst){
                ((*inst).*f)({data, size});
            }, client);
        });
        return *this;
    }

    using FullFunc = MemberFunc<std::string_view, std::string_view, std::string_view>;
    template<StringLiteral Prefix, StringLiteral Suffix = kEndl>
    auto& add(FullFunc func) requires std::derived_from<T, AtCompBase> {
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

  private:
    template<class Slots>
    struct ArgMapper { };

    template<class SlotType, class SlotCntType, SlotCntType SlotCnt>
    struct ArgMapper<LimitedSlots<SlotType, SlotCntType, SlotCnt>> {
        ArgMapper(std::shared_ptr<T> inst, std::optional<Offset<T, LimitedSlots<SlotType, SlotCntType, SlotCnt>>> slotOffset): inst(inst), slotOffset(slotOffset) { }

        template<class X> requires std::same_as<std::shared_ptr<SlotType>, X>
        std::tuple<X, bool> operator()(X x, std::string_view a) {
            slotId = parse<int>(a);
            if(!slotOffset)
                throw std::runtime_error{"slots not set"};
            return {slotOffset->recover(*inst).acuqire(*slotId), true};
        }

        template<class X>
        struct EventBuilderInfo: public std::false_type { };

        template<class E>
        struct EventBuilderInfo<EventBuilder<E>>: public std::true_type {
            using Element = E;
        };

        template<class X> requires(EventBuilderInfo<X>::value)
        std::tuple<X, bool> operator()(X x, std::string_view a) {
            return {X(events), false};
        }

        template<class X> requires(!std::same_as<std::shared_ptr<SlotType>, X> && !EventBuilderInfo<X>::value)
        std::tuple<X, bool> operator()(X x, std::string_view a) {
            return {parse<X>(a), true};
        }

        std::optional<int> getSlotId() {
            return slotId;
        }

        std::vector<int> getEvents() {
            return move(events);
        }

      private:
        std::shared_ptr<T> inst;
        std::optional<Offset<T, LimitedSlots<SlotType, SlotCntType, SlotCnt>>> slotOffset;
        std::optional<int> slotId;
        std::vector<int> events = {};
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

    static constexpr auto kDefaultOpt = (typename Options::Values)(Options::UnwrapQuota | Options::RemoveLeadingPad);

    template<class R, class... Args>
    using ProbedMemberFunc = R(T::*)(Args...);

    template<class R, class... Args>
    using ProbedLambdaFunc = std::function<R(Args...)>;

    template<class F> struct IsProbedMemberFunc: public std::false_type { };
    template<class R, class... Args> struct IsProbedMemberFunc<ProbedMemberFunc<R, Args...>>: public std::true_type { };
    template<class F> auto wrapUp(F func) requires(IsProbedMemberFunc<F>::value) { return func; }
    template<class F> auto wrapUp(F func) requires(!IsProbedMemberFunc<F>::value) { return std::function{func}; }

    template<StringLiteral Prefix, StringLiteral Suffix, class F>
    auto& probe(F func, typename Options::Values opt = kDefaultOpt) {
        probeI<Prefix, Suffix>(wrapUp(func), defaultSlotsOffset, defaultEventOffset, std::nullopt, opt);
        return *this;
    }

    template<StringLiteral Prefix, StringLiteral Suffix, class F>
    auto& probe(F func, int offset, typename Options::Values opt = kDefaultOpt) {
        probeI<Prefix, Suffix>(wrapUp(func), defaultSlotsOffset, defaultEventOffset, offset, opt);
        return *this;
    }

    template<StringLiteral Prefix, StringLiteral Suffix, class SlotType, class SlotCntType, SlotCntType SlotCnt, class F>
    auto& probe(F func, LimitedSlots<SlotType, SlotCntType, SlotCnt>& slots, typename Options::Values opt = kDefaultOpt) {
        probeI<Prefix, Suffix>(wrapUp(func), std::optional{Offset(*(T*)comp, slots)}, defaultEventOffset, std::nullopt, opt);
        return *this;
    }

    template<StringLiteral Prefix, StringLiteral Suffix, class SlotType, class SlotCntType, SlotCntType SlotCnt, class F>
    auto& probe(F func, LimitedSlots<SlotType, SlotCntType, SlotCnt>& slots, int offset, typename Options::Values opt = kDefaultOpt) {
        probeI<Prefix, Suffix>(wrapUp(func), std::optional{Offset(*(T*)comp, slots)}, defaultEventOffset, offset, opt);
        return *this;
    }

    template<StringLiteral Prefix, StringLiteral Suffix, class SlotType, class SlotCntType, SlotCntType SlotCnt, class F>
    auto& probe(F func, LimitedSlots<SlotType, SlotCntType, SlotCnt>& slots, EventSet& event, int offset, typename Options::Values opt = kDefaultOpt) {
        probeI<Prefix, Suffix>(wrapUp(func), std::optional{Offset(*(T*)comp, slots)}, std::optional{Offset(*(T*)comp, event)}, offset, opt);
        return *this;
    }

  private:
    template<StringLiteral Prefix, StringLiteral Suffix, class SlotType, class SlotCntType, SlotCntType SlotCnt, class F>
    void probeI(
      F func, 
      std::optional<Offset<T, LimitedSlots<SlotType, SlotCntType, SlotCnt>>> slotsOffset, 
      std::optional<Offset<T, EventSet>> eventOffset, 
      std::optional<int> offset = {}, 
      typename Options::Values opt = kDefaultOpt) {
        static auto f = func;
        static auto o = (int)opt;
        static auto oSlots = slotsOffset;
        static auto oEvent = eventOffset;
        static auto ost = offset;
        AtUrcBuilderBase::add(Prefix.value, Suffix.value, [](at_client_t client, const char* data, rt_size_t size) {
            using Mapper = ArgMapper<LimitedSlots<SlotType, SlotCntType, SlotCnt>>;
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

    template<class R, class B, class Arg>
    struct ProbeResultHandler {

        ProbeResultHandler(B bound, Arg arg): bound(std::move(bound)), arg(std::move(arg)) {}

        std::vector<int> operator()() requires(std::is_void_v<R>) {
            std::apply(bound, arg);
            return {};
        }

        std::vector<int> operator()() requires EnumOrIntergral<R> {
            auto r = std::apply(bound, arg);
            return {r};
        }

        enum class Type { Unknown, Optional, Vector };
        template<class> struct Info { static constexpr Type kType = Type::Unknown; };
        template<class E> struct Info<std::optional<E>> { static constexpr Type kType = EnumOrIntergral<E> ? Type::Optional : Type::Unknown; };
        template<class E> struct Info<std::vector<E>> { static constexpr Type kType = EnumOrIntergral<E> ? Type::Vector : Type::Unknown; };

        std::vector<int> operator()() requires(Info<R>::kType == Type::Optional) {
            auto r = std::apply(bound, arg);
            auto result = std::vector<int>{};
            if(r) result.push_back(*r);
            return result;
        }

        std::vector<int> operator()() requires(Info<R>::kType == Type::Vector) {
            auto r = std::apply(bound, arg);
            return r;
        }

      private:
        B bound;
        Arg arg;
    };

    template<class F> struct ProbedFuncInfo;
    template<class R, class... Args>
    struct ProbedFuncInfo<ProbedMemberFunc<R, Args...>> {
        static constexpr int kArgsSize = sizeof...(Args);
        using Tuple = std::tuple<Args...>;

        static auto tryBind(ProbedMemberFunc<R, Args...> f, std::shared_ptr<T> inst) {
            return std::bind_front(f, inst.get());
        }
    };

    template<class R, class... Args>
    struct ProbedFuncInfo<ProbedLambdaFunc<R, Args...>> {
        static constexpr int kArgsSize = sizeof...(Args);
        using Tuple = std::tuple<Args...>;

        static auto tryBind(ProbedLambdaFunc<R, Args...> f, std::shared_ptr<T> inst) {
            return f;
        };
    };

    template<StringLiteral Prefix, StringLiteral Suffix, class M, class F>
    static void runProbe(std::function<M(std::shared_ptr<T>)> maker, F f, at_client_t client, const char* data, rt_size_t size, int opt, std::function<void(std::shared_ptr<T>, M&, std::vector<int>)> then = {}) {
        run([&](auto inst){
            using Info = ProbedFuncInfo<F>;
            auto argStrs = getArgs<Prefix, Suffix>(data, size, Info::kArgsSize);
            auto m = maker(inst);
            int i = 0;
            auto zipper = [&](auto x) {
                auto&& [r, next] = m(x, PreprocessArg(std::string_view{argStrs[i]}, opt));
                if(next) i++;
                return r;
            };
            auto args = std::apply([&](auto ...x) {
                return std::make_tuple(zipper(x)...);
            }, typename Info::Tuple()); //TODO: 这里要重构
            auto bound = Info::tryBind(f, inst);
            auto handler = ProbeResultHandler<decltype(std::apply(bound, args)), decltype(bound), decltype(args)>(std::move(bound), std::move(args));
            if(then) then(inst, m, handler());
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