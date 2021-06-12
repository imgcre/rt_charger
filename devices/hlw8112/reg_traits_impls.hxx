#pragma once
#include "reg_traits.hxx"
#include "regs.hxx"

namespace Hlw8112N {

template<>
struct RegTraits<SysCon> {
    static constexpr auto meta = RegMeta<SysCon> {
        initial: {
            pgaIB: SysCon::PGA::x16,
            adc2On: true,
        },
        addr: 0x00,
    };
};

template<>
struct RegTraits<EmuCon> {
    static constexpr auto meta = RegMeta<EmuCon> {
        initial: {
            compOff: true,
        },
        addr: 0x01,
    };
};

template<>
struct RegTraits<EmuCon2> {
    static constexpr auto meta = RegMeta<EmuCon2> {
        initial: {
            chsIB: true,
        },
        addr: 0x13,
    };
};

template<>
struct RegTraits<RmsIA> {
    static constexpr auto meta = RegMeta<RmsIA> {
        addr: 0x24,
        size: 3,
        readonly: true,
    };
};

template<>
struct RegTraits<RmsIB> {
    static constexpr auto meta = RegMeta<RmsIB> {
        addr: 0x25,
        size: 3,
        readonly: true,
    };
};

template<>
struct RegTraits<RmsU> {
    static constexpr auto meta = RegMeta<RmsU> {
        addr: 0x26,
        size: 3,
        readonly: true,
    };
};

template<>
struct RegTraits<RmsIAC> {
    static constexpr auto meta = RegMeta<RmsIAC> {
        addr: 0x70,
        readonly: true,
    };
};

template<>
struct RegTraits<RmsIBC> {
    static constexpr auto meta = RegMeta<RmsIBC> {
        addr: 0x71,
        readonly: true,
    };
};

template<>
struct RegTraits<RmsUC> {
    static constexpr auto meta = RegMeta<RmsUC> {
        addr: 0x72,
        readonly: true,
    };
};

}