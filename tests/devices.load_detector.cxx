#include <devices/load_detector.hxx>
#include <rtthread.h>
#include <configs/bsp.hxx>
#include <utilities/mp.hxx>
#include <ranges>
using namespace std;

static int init_test_load_detector() {
    for(auto i: views::iota(0, Config::Bsp::kPortNum)) {
        magic_switch<Config::Bsp::kPortNum>{}([&](auto v) {
            auto lodet = Preset::LoadDetector<decltype(v)::value>::get();
            lodet->oState += [=](auto value){
                if(!value) return;
                rt_kprintf("ldt[%d] -> %d\n", i, value);
            };
        }, i);
    }
    return RT_EOK;
}

INIT_APP_EXPORT(init_test_load_detector);