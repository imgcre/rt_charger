#pragma once
#include <utilities/smart_nested.hxx>
#include <cstddef>
#include <chrono>

class CompactedEventSet;
class Event: public SmartNested<CompactedEventSet> {
    friend class CompactedEventSet;
  private:
    Event(OuterPtr outer, uint8_t bit);
    Event(const Event& other) = delete;
  public:
    ~Event();
    void set();
    void wait(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());
  private:
    uint8_t bit;
};