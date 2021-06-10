#include "raw_port.hxx"
#include <configs/bsp.hxx>
#include <cstdint>

bool RawPort::validate() const {
    return 0 <= value && value < Config::Bsp::kPortNum;
}