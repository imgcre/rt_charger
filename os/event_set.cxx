#include "event_set.hxx"
using namespace std;

void EventSet::set(uint32_t e) {
    rt_event_send(event.get(), e);
}

void EventSet::waitAll(uint32_t e, chrono::milliseconds timeout) {
    int toi = timeout == timeout.max() ? RT_WAITING_FOREVER : timeout.count();
    wait(e, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, toi, nullptr);
}