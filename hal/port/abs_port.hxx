#pragma once
#include <cstdint>
#include "port_base.hxx"
#include "raw_port.hxx"
#include <configs/bsp.hxx>

class RawPort;

class AbsPort: public PortBase {
    using PortBase::PortBase;
  public:
    constexpr operator RawPort() const;
    bool validate() const;
};

constexpr AbsPort operator "" _ap(unsigned long long n) {
  return AbsPort{(uint8_t)n};
}