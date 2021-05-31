#include "port.hxx"

Port::Port(uint8_t value): value(value) {

}

uint8_t Port::get() const {
    return value;
}