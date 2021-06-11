#pragma once
#include "event.hxx"
#include <bitset>
#include <rtthread.h>
#include <chrono>

class CompactedEventSet: public std::enable_shared_from_this<CompactedEventSet> {
    friend class Event;
    static constexpr std::size_t N = 32;
  public:
    CompactedEventSet() = default;
    CompactedEventSet(const CompactedEventSet&) = delete;
    ~CompactedEventSet();
    bool full();
    Event alloc();
  private:
    void free(uint8_t bit);
    void set(uint8_t bit);
    void wait(uint8_t bit, std::chrono::milliseconds timeout);

  private:
    std::bitset<N> bits = {};
    rt_event_t event = rt_event_create("ces", RT_IPC_FLAG_FIFO);
};
