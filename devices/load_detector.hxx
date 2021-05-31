#pragma once

#include <rtthread.h>
#include <array>
#include <variant>
#include <memory>
#include <os/observable.hxx>
#include <os/timer.hxx>

class LoadDetector {
public:
    LoadDetector(rt_base_t pin);

private:
    rt_base_t pin;

    //kInvdThrDurationMs内少于kInvdThrNMin有效信号则无效
    //参数调整:
public:
    const static int
        kPluseFreqHz = 50, //脉冲频率为市电频率
        kDetectWndMs = 200, //检测窗口
        kValidRatioPc = 60, //有效比率
        kArraySize = kDetectWndMs / (1000 / kPluseFreqHz);

    std::array<rt_int64_t, kArraySize> ticks;
    bool fulled = false;
    int rear = 0;
    Observable<std::optional<bool>> state;
public:
    Observer<std::optional<bool>> oState;
private:
    static const char* kTimer;
    static Timer timer;
};

#include <utilities/singleton.hxx>
#include <configs/bsp.hxx>
namespace Preset {
template<int R>
class LoadDetector: public Singleton<LoadDetector<R>>, public Config::Bsp::assert_t<::LoadDetector, R> {
    friend class Singleton<LoadDetector<R>>;
    LoadDetector(): ::LoadDetector(kPin) {}
    static const rt_base_t kPin;
};
}
