#pragma once

#include <rtthread.h>
#include <rtdevice.h>
#include <optional>
#include <os/observable.hxx>
#include "output_pin.hxx"

class Relay {
public:

    enum class Polar {
        High, Low,
    };

    Relay(rt_base_t pin, Polar polar);

    enum Value {
        On, Off,
    };

public:
    Observable<std::optional<Value>> value = {};

private:
    OutputPin pin;
    const Polar polar;
    const bool onPinVal = polar == Polar::High ? true : false;
    const bool offPinVal = !onPinVal;
};

#include <utilities/singleton.hxx>
#include <configs/bsp.hxx>
#include <hal/port.hxx>
namespace Preset {
template<AbsPort R>
class Relay: public Singleton<Relay<R>, ::Relay>, public ::Relay {
    friend class Singleton<Relay<R>, ::Relay>;
    Relay(): ::Relay(kPin, ::Relay::Polar::Low) {}
    static const rt_base_t kPin;
};
}