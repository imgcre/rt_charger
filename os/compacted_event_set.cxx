#include "compacted_event_set.hxx"
#include "event_allocator.hxx"
#include <stdexcept>


using namespace std;

bool CompactedEventSet::full() {
    return bits.all();
}

Event CompactedEventSet::alloc() {
    if(full()) throw runtime_error{"fulled"};
    uint8_t currBit = 0u;
    for(uint8_t i = 0u; i < bits.size(); i++) {
        if(!bits.test(i)) {
            currBit = i;
            break;
        }
    }
    bits.set(currBit);
    return Event{shared_from_this(), currBit};
}

CompactedEventSet::~CompactedEventSet() {
    rt_event_delete(event);
}

void CompactedEventSet::free(uint8_t bit) {
    bits.reset(bit);
    if(bits.none()) {
        EventAllocator::dispose(shared_from_this());
    }
}

void CompactedEventSet::set(uint8_t bit) {
    rt_event_send(event, 1 << bit);
}

void CompactedEventSet::wait(uint8_t bit, chrono::milliseconds timeout) {
    auto rt_timeout = timeout == chrono::milliseconds::max() ? RT_WAITING_FOREVER : timeout.count();
    auto ret = rt_event_recv(event, 1 << bit, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, rt_timeout, nullptr);
    if(ret != RT_EOK) {
        throw runtime_error{"event timeout"};
    }
}