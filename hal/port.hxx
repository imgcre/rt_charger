#pragma once
#include "port/abs_port.hxx"
#include "port/raw_port.hxx"
#include "port/port_preset.hxx"
#include <type_traits>
#include <configs/bsp.hxx>
#include <utility>
#include <stdexcept>

constexpr RawPort::operator AbsPort() const {
    return AbsPort{uint8_t(value + 1)};
}

constexpr AbsPort::operator RawPort() const {
    return RawPort{uint8_t(value - 1)};
}

template<AbsPort P>
struct PortWrapper {
    static constexpr auto Port = P;
};

struct Port {
    template<class F, int N = 0>
    static void forEach(F&& f) {
        if constexpr(N < Config::Bsp::kPortNum) {
            f(PortWrapper<RawPort(N)>{});
            forEach<F, N + 1>(std::forward<F>(f));
        }
    }

    template<class F, int N = 0>
    static void select(F&& f, RawPort p) {
        if constexpr(N < Config::Bsp::kPortNum) {
            if(N == p.get()) {
                f(PortWrapper<RawPort(N)>{});
            } else {
                select<F, N + 1>(std::forward<F>(f), p);
            }
        }
    }

    template<template <AbsPort> class P, int N = 0>
    static auto to(RawPort p) -> decltype(P<RawPort(N)>::get()) {
        if constexpr(N < Config::Bsp::kPortNum) {
            if(N == p.get()) {
                return P<RawPort(N)>::get();
            } else {
                return to<P, N + 1>(p);
            }
        }
        return {};
    }
};