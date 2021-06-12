#include "uart.hxx"
#include <stdexcept>
#include <rthw.h>
#include <os/timeout.hxx>

Uart::Uart(const char* deviceName, struct serial_configure* conf) {
    device = rt_device_find(deviceName);
    if(conf != nullptr)
        rt_device_control(device, RT_DEVICE_CTRL_CONFIG, conf);
    rt_device_open(device, RT_DEVICE_FLAG_INT_TX);
    Indicator::setup(device, &Uart::onRecv);
}

int Uart::read(std::byte* data, int len, std::chrono::milliseconds timeout) {
    auto helper = Timeout{timeout};
    while(rxRemain < len) {
        event.wait(helper.remain());
    }
    rxRemain -= len;
    rt_device_read(device, 0, data, len);
    return len;
}

void Uart::write(std::byte* data, int len) {
    
}

void Uart::onRecv(std::size_t size) {
    event.set();
    rxRemain = size;
}