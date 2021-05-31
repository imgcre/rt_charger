#include "abs_port.hxx"
#include <cstdint>
AbsPort::operator RawPort() const {
    return RawPort{uint8_t(value - 1)};
}

bool AbsPort::validate() const {
    return ((RawPort)*this).validate();
}
