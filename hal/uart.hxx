#pragma once
#include <dp/iostream.hxx>
#include <rx_indicator.hxx>
#include <rtthread.h>
#include <os/event_allocator.hxx>
#include <atomic>

class Uart: public IOStream, public RxIndicator<Uart> {
  public:
    Uart(const char* deviceName, struct serial_configure* conf = nullptr);
    Uart(const Uart&) = delete;

    //read all or read none
    virtual int read(std::byte* data, int len, std::chrono::milliseconds timeout) override;
    virtual void write(std::byte* data, int len) override;
  private:
    void onRecv(std::size_t size);

  private:
    rt_device_t device;
    Event event = EventAllocator::alloc();
    std::atomic<std::size_t> rxRemain = 0;
};