#include "abs_port.hxx"
#include <cstdint>
#include "port.hxx"

bool AbsPort::validate() const {
    return ((RawPort)*this).validate();
}
