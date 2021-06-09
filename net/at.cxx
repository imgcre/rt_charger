#include "at.hxx"
#include <rtdevice.h>

At::At(const char* deviceName, std::size_t bufSize) {
    struct serial_configure conf = RT_SERIAL_CONFIG_DEFAULT;
    conf.bufsz = bufSize;
    auto serial = rt_device_find(deviceName);
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &conf);
    at_client_init(deviceName, bufSize);
    atClient = at_client_get(deviceName);
    atClient->device->user_data = this;
}

int At::waitConn(uint32_t timeout) {
    auto ret = at_client_obj_wait_connect(atClient, timeout);
    if(ret != RT_EOK) {
        throw AtExecError{ret};
    }
    return ret;
}

void At::addUrc(std::vector<at_urc> urcs) {
    urcList.push_back(std::move(urcs));
    auto& target = urcList.back();
    at_obj_set_urc_table(atClient, &target[0], target.size());
}