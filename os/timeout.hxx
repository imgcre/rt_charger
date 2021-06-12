#pragma once
#include <rtthread.h>
#include <chrono>

class Timeout {
  public:
    Timeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds remain();

  private:
    rt_tick_t expired;
};