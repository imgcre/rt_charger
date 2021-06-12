#pragma once
#include <cstdint>

namespace Hlw8112N {
struct SysCon {
    enum class PGA: uint16_t { x1, x2, x4, x8, x16 };

    PGA pgaIA: 3 = PGA::x16;
    PGA pgaU: 3 = PGA::x1;
    PGA pgaIB: 3 = PGA::x1;
    uint16_t adc1On: 1 = true;
    uint16_t adc2On: 1 = false;
    uint16_t adc3On: 1 = true;
};

struct EmuCon {
    enum class PMode: uint16_t { DoubleSum, PositiveOnly, DoubleAbs };
    enum class TSensorStep: uint16_t { Step1, Step2, Step3, Step4 };

    uint16_t pARun: 1 = false;
    uint16_t pBRun: 1 = false;
    uint16_t reserved1: 2 = 0;
    uint16_t hpfUOff: 1 = false;
    uint16_t hpfIAOff: 1 = false;
    uint16_t hpfIBOff: 1 = false;
    uint16_t zxd0: 1 = 0;
    uint16_t zxd1: 1 = 0;
    uint16_t dcMode: 1 = false;
    PMode pMode: 2 = PMode::DoubleSum;
    uint16_t compOff: 1 = false;
    uint16_t tSensorEn: 1 = false;
    TSensorStep tSensorStep: 2 = TSensorStep::Step1;
};

struct EmuCon2 {
    enum class VRefSel: uint16_t { Invalid, v1_25Ref };
    enum class DupSel: uint16_t { f3_4Hz, f6_8Hz, f13_65Hz, f27_3Hz };
    enum class Sdo: uint16_t { Cmos, CmosOpenDrain };

    VRefSel vRefSel: 1 = VRefSel::v1_25Ref;
    uint16_t peakEn: 1 = false;
    uint16_t zxEn: 1 = false;
    uint16_t overEn: 1 = false;
    uint16_t sagEn: 1 = false;
    uint16_t waveEn: 1 = false;
    uint16_t pFactorEn: 1 = false;
    uint16_t chsIB: 1 = false;
    DupSel dupSel: 2 = DupSel::f3_4Hz;
    uint16_t epaCb: 1 = false;
    uint16_t epbCb: 1 = false;
    Sdo sdoCmos: 1 = Sdo::Cmos;
    uint16_t reserved2: 2 = 0;
    uint16_t dotpSel: 1 = 0;
};

struct RmsIA {
    uint32_t data: 24;
};

struct RmsIB {
    uint32_t data: 24;
};

struct RmsU {
    uint32_t data: 24;
};

struct RmsIAC {
    uint16_t value;
};

struct RmsIBC {
    uint16_t value;
};

struct RmsUC {
    uint16_t value;
};

}