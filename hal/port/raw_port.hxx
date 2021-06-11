#pragma once
#include "port_base.hxx"
#include "abs_port.hxx"

class AbsPort;

class RawPort: public PortBase {
    using PortBase::PortBase;
  public:
    constexpr operator AbsPort() const;
    bool validate() const;

};

constexpr RawPort operator "" _rp(unsigned long long n) {
  return RawPort{(uint8_t)n};
}