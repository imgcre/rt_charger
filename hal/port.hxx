#pragma once
#include <cstdint>

class Port {
  public:
    explicit Port(uint8_t value);
    uint8_t get() const;
    virtual bool validate() const = 0;
    
  protected:
    uint8_t value;
};