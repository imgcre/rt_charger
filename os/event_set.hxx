#pragma once
#include <rtthread.h>
#include <cstddef>
#include <stdexcept>
#include <memory>
#include <os/rt_error.hxx>
#include <chrono>

class EventSet {
  public:
    EventSet() = default;
    void set(uint32_t e);

    template<class... Args>
    void wait(uint32_t e, Args&&... args) {
        auto result = rt_event_recv(event.get(), e, std::forward<Args>(args)...);
        if(result != RT_EOK) {
            throw RtError{result};
        }
    }

    void waitAll(uint32_t e, std::chrono::milliseconds timeout = std::chrono::milliseconds::max());
  private:
    std::shared_ptr<rt_event> event = std::shared_ptr<rt_event>(rt_event_create("es", RT_IPC_FLAG_FIFO), [](auto p) {
      rt_event_delete(p);
    });
};