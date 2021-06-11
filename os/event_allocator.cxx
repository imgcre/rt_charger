#include "event_allocator.hxx"
#include <algorithm>
#include <Lock.h>
#include <critical.hxx>

using namespace std;
using namespace rtthread;

Event EventAllocator::alloc() {
    auto guard = Critical{};
    auto avaliableEventSet = shared_ptr<CompactedEventSet>{};
    for(auto eventSet: eventSets) {
        auto locked = eventSet.lock();
        if(!locked) continue;
        if(!locked->full()) {
            avaliableEventSet = locked;
            break;
        }
    }
    if(!avaliableEventSet) {
        avaliableEventSet = shared_ptr<CompactedEventSet>{new CompactedEventSet()};
        eventSets.push_back(avaliableEventSet);
    }
    return avaliableEventSet->alloc();
}

void EventAllocator::dispose(std::shared_ptr<CompactedEventSet> eventSet) {
    auto guard = Critical{};
    eventSets.remove_if([&](auto currSet) {
        auto locked = currSet.lock();
        if(!locked || locked == eventSet) {
        }
        return !locked || locked == eventSet;
    });
}

std::list<std::weak_ptr<CompactedEventSet>> EventAllocator::eventSets = {};