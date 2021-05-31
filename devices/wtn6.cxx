#include "wtn6.hxx"

#include <rtdevice.h>
#include <rthw.h>

Wtn6::Wtn6(rt_base_t dataPin, rt_base_t busyPin, std::shared_ptr<Task> task): dataPin(dataPin), busyPin(busyPin), task(task) {
    rt_pin_mode(dataPin, PIN_MODE_OUTPUT);
    rt_pin_mode(busyPin, PIN_MODE_INPUT);
    rt_pin_write(dataPin, PIN_HIGH);
}

bool Wtn6::isBusy() {
    return rt_pin_read(busyPin) == PIN_LOW;
}

void Wtn6::write(rt_uint8_t data) {
    task->exec([this, self = shared_from_this(), data]{
        rt_pin_write(dataPin, PIN_LOW);
        rt_thread_mdelay(5);
        rt_pin_write(dataPin, PIN_HIGH);
        for(auto i = 0; i < 8; i++) {
            writeBit((data & (1 << i)) != 0);
        }
        rt_pin_write(dataPin, PIN_HIGH);
    });
}

void Wtn6::writeBit(bool bit) {
    auto delayHigh = 200, delayLow = 600;
    if(bit) {
        delayHigh = 600;
        delayLow = 200;
    }

    rt_pin_write(dataPin, PIN_HIGH);
    rt_hw_us_delay(delayHigh);
    rt_pin_write(dataPin, PIN_LOW);
    rt_hw_us_delay(delayLow);
}
