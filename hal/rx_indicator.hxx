#pragma once
#include <rtthread.h>
#include <cstddef>

template<class T>
class RxIndicator {
    using MemberCallback = void(T::*)(std::size_t size);
  protected:
    using Indicator = RxIndicator<T>;
    void setup(rt_device_t device, MemberCallback callback) {
        device->user_data = this;
        this->callback = callback;

        rt_device_set_rx_indicate(device, [](auto dev, auto size) -> rt_err_t {
            auto self = (T*)dev->user_data;
            if(self->callback) {
                (self->*(self->callback))(size);
            }
            return RT_EOK;
        });
    }

  private:
    MemberCallback callback;
};