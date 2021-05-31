#include "load_detector.hxx"
#include <rtdevice.h>
using namespace std;

LoadDetector::LoadDetector(rt_base_t pin): pin(pin), oState(state) {
    rt_pin_mode(pin, PIN_MODE_INPUT_PULLDOWN);
    rt_pin_attach_irq(pin, PIN_IRQ_MODE_RISING, [](auto p) {
        auto self = (LoadDetector*)p;
        self->ticks[self->rear] = rt_tick_get();
        self->rear++;
        self->rear %= self->kArraySize;

        if(!self->fulled && self->rear == 0) {
            self->fulled = true;
        }
    }, this);
    rt_pin_irq_enable(pin, PIN_IRQ_ENABLE);

    timer.onRun += [this]{
        auto size = kArraySize;
        if(!fulled)
            size = rear;
        auto now = rt_tick_get();
        auto count = int{};
        for(auto i = 0; i < size; i++) {
            if(now - ticks[i] < kDetectWndMs)
                count++;
        }
        state = count >= kArraySize * kValidRatioPc / 100;
    };

    timer.start();
}

const char* LoadDetector::kTimer = "lodet";
Timer LoadDetector::timer = {kDetectWndMs, kTimer};
