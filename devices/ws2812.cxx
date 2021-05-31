#include "ws2812.hxx"
#include <rtdevice.h>
#include <stm32f1xx.h>
#include <devices/pin_map.hxx>

Ws2812::Ws2812(rt_base_t dinPin, int num, std::shared_ptr<Task> task): dinPin(dinPin), frame(num), bssr(&(PIN_STPORT(dinPin))->BSRR), pin(PIN_STPIN(dinPin)), task(task) {
    rt_pin_mode(dinPin, PIN_MODE_OUTPUT);
    rt_pin_write(dinPin, PIN_HIGH);
}

std::shared_ptr<void> Ws2812::getSess() {
    return std::shared_ptr<void>(nullptr, [this](auto) {
        flush();
    });
}

void Ws2812::flush() {
    task->exec([this](){
        reset();
        for(auto i = 0u; i < frame.size(); i++) {
            writePixel(frame[i]);
        }
    });
}

void Ws2812::reset() {
    rt_hw_us_delay(80);
}

Colors::Rgb& Ws2812::getPixel(int idx) {
    return frame[idx];
}

std::size_t Ws2812::getCount() {
    return frame.size();
}

void Ws2812::writePixel(Colors::Rgb pixel) {
    writeByte(pixel.r);
    writeByte(pixel.g);
    writeByte(pixel.b);
}

void Ws2812::writeByte(rt_uint8_t b) {
    for(auto i = 7; i >= 0; i--) {
        writeBit(b & (1 << i));
    }
}

void Ws2812::writeBit(int value) {
    auto ctx = rt_hw_interrupt_disable();
    *bssr = pin;
    if(value) {
        delay<31>();
        *bssr = pin << 16;
        delay<6>();
    } else {
        delay<10>();
        *bssr = pin << 16;
        delay<23>();
    }
    rt_hw_interrupt_enable(ctx);
}
