#include "at.hxx"

At::At(const char* deviceName, std::size_t bufSize) {
    at_client_init(deviceName, bufSize);
    atClient = at_client_get(deviceName);
    atClient->device->user_data = this;
}

int At::waitConn(uint32_t timeout) {
    auto ret = at_client_obj_wait_connect(atClient, timeout);
    if(ret != RT_EOK) {
        throw AtError{ret};
    }
    return ret;
}