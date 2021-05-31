#pragma once
#include "port.hxx"
#include "abs_port.hxx"

class AbsPort;

class RawPort: public Port {
    using Port::Port;
  public:
    operator AbsPort() const;
    virtual bool validate() const final;

};
