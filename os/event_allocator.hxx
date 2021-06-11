#pragma once
#include "compacted_event_set.hxx"
#include <list>
#include <memory>
#include <Mutex.h>

class EventAllocator {
    friend class CompactedEventSet;
  public:
    static Event alloc();
  private:
    static void dispose(std::shared_ptr<CompactedEventSet> eventSet);

    static std::list<std::weak_ptr<CompactedEventSet>> eventSets;
};