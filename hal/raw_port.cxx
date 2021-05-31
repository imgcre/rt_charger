#include "raw_port.hxx"
#include <configs/bsp.hxx>
#include <cstdint>

RawPort::operator AbsPort() const {
    return RawPort{uint8_t(value + 1)};
}

bool RawPort::validate() const {
    return 0 <= value && value < Config::Bsp::kPortNum;
}