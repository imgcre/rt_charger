#pragma once
#include <cstddef>
#include <stdexcept>
#include <array>
#include <memory>
#include <tuple>
#include <iterator>
#include <algorithm>

template<class SlotType, class SlotCntType, SlotCntType SlotCnt>
class LimitedSlots {
  public:
    std::tuple<std::weak_ptr<SlotType>&, SlotCntType> getNext() {
        for(auto i = 0u; i < slots.size(); i++) {
            auto& slot = slots[i];
            if(slot.expired()) {
                return {slot, i};
            }
        }
        throw std::runtime_error{"slot exhausted"};
    }

    std::shared_ptr<SlotType> acuqire(SlotCntType idx) {
        auto slot = slots[idx].lock();
        if(slot == nullptr) {
            throw std::runtime_error{"slot is invalid"};
        }
        return slot;
    }

    SlotCntType idOf(std::shared_ptr<SlotType> slot) {
        for(auto i = 0; i < slots.size(); i++) {
            auto currSlot = slots[i].lock();
            if(currSlot && currSlot == slot) {
                return i;
            }
        }
        throw std::runtime_error{"slot not found"};
    }

  private:
    std::array<std::weak_ptr<SlotType>, SlotCnt> slots = {};
};

template<class SlotType>
using SingleSlot = LimitedSlots<SlotType, uint8_t, 1>;
