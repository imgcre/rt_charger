#include <rtthread.h>
#include <rtdevice.h>
#include <at.h>
#include <board.h>

constexpr auto kPwrPin = GET_PIN(B, 2);

int init_test_at() {
    rt_pin_mode(kPwrPin, PIN_MODE_OUTPUT);
    rt_pin_write(kPwrPin, PIN_HIGH);
    struct serial_configure conf = RT_SERIAL_CONFIG_DEFAULT;
    rt_base_t bufsz = 512;
    conf.bufsz = bufsz;
    conf.baud_rate = BAUD_RATE_115200;
    auto serial = rt_device_find("uart2");
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &conf);
    at_client_init("uart2", bufsz);
    return RT_EOK;
}

INIT_APP_EXPORT(init_test_at);