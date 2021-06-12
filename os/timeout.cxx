#include "timeout.hxx"

using namespace std::chrono;

Timeout::Timeout(milliseconds timeout) {
    if(timeout == milliseconds::max()) {
        expired = RT_WAITING_FOREVER;
    } else {
        expired = rt_tick_get() + timeout.count();
    }
}

std::chrono::milliseconds Timeout::remain() {
    if(expired == RT_WAITING_FOREVER) {
        return milliseconds::max();
    }
    auto val = (long long)expired - rt_tick_get();
    if(val < 0) {
        val = 0;
    }
    return milliseconds(val);
}