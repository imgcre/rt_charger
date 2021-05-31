#pragma once

#include <optional>
#include <rtthread.h>
#include <os/observable.hxx>

class OutputPin {
    public:
    using handler_t = Observable<std::optional<bool>>;

    explicit OutputPin(rt_base_t pin);
    handler_t& getHandler();

    operator handler_t&();
    handler_t& operator*();

private:
    rt_base_t pin;
    handler_t handler;
    Observable<bool> inited = {false};
};