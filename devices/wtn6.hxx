#pragma once

#include <rtthread.h>

class Wtn6 {
  public:
    Wtn6(rt_base_t dataPin, rt_base_t busyPin);

    bool isBusy();
    void write(rt_uint8_t data);

  private:
    const rt_base_t dataPin, busyPin;
};

#include <utilities/singleton.hxx>
namespace Preset {
class Wtn6: public Singleton<Wtn6>, public ::Wtn6 {
    friend singleton_t;
    Wtn6(): ::Wtn6(kDataPin, kBusyPin) {}
    static const rt_base_t kDataPin, kBusyPin;
};
}