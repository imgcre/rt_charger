#pragma once

#include <memory>
#include <rtthread.h>
#include <os/task.hxx>

//! preset
class Wtn6: public std::enable_shared_from_this<Wtn6> {
  public:
    Wtn6(rt_base_t dataPin, rt_base_t busyPin, std::shared_ptr<Task> task);

    bool isBusy();
    void write(rt_uint8_t data);

  private:
    void writeBit(bool bit);
  private:
    //! config('bsp')
    const rt_base_t dataPin, busyPin;
    //! use('Priority::High')
    std::shared_ptr<Task> task;
};

#include <utilities/singleton.hxx>
namespace Preset {
class Wtn6: public Singleton<Wtn6>, public ::Wtn6 {
    friend singleton_t;
    Wtn6(): ::Wtn6(kDataPin, kBusyPin, Task<Priority::High>::get()) {}
    static const rt_base_t kDataPin, kBusyPin;
};
}