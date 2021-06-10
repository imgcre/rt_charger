#pragma once
#include <cstdint>

class PortBase {
  public:
    constexpr explicit PortBase(uint8_t value): value(value) { }
    uint8_t get() const;
    
  public:
    const uint8_t value;
};