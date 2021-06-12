#pragma once
#include <rthw.h>

class InterruptDisabler {
  public:
    inline InterruptDisabler(): level(rt_hw_interrupt_disable()) { }

    inline ~InterruptDisabler() {
        rt_hw_interrupt_enable(level);
    }
  private:
    rt_base_t level;
};