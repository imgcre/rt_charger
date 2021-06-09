#include <rtdevice.h>
#include "air302.hxx"
#include "air302/basic.hxx"

Air302::Air302(const char* deviceName, std::size_t bufSize, rt_base_t pwrPin): Base(deviceName, bufSize), pwrPin(pwrPin) {
    rt_pin_mode(pwrPin, PIN_MODE_OUTPUT);
    rt_pin_write(pwrPin, PIN_HIGH);
}

void Air302::init() {
    if(inited) return;
    inited = true;
    waitConn();
}