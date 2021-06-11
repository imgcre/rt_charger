#include "compacted_event_set.hxx"
#include "event.hxx"

Event::Event(OuterPtr outer, uint8_t bit): Base(outer), bit(bit) {

}

Event::~Event() {
    outer->free(bit);
}

void Event::set() {
    outer->set(bit);
}

void Event::wait(std::chrono::milliseconds timeout) {
    outer->wait(bit, timeout);
}