#pragma once
#include <cstdint>
#include "port.hxx"
#include "raw_port.hxx"

class RawPort;

class AbsPort: public Port {
    using Port::Port;
  public:
    operator RawPort() const;
    virtual bool validate() const final;


};