#include <devices/load_detector.hxx>
#include <rtthread.h>
#include <configs/bsp.hxx>
#include <utilities/mp.hxx>
#include <ranges>
using namespace std;

static int init_load_detector() {
    Port::forEach([]<class T>(T){
        auto lodet = Preset::LoadDetector<T::Port>::get();
        lodet->oState += [=](auto value){
            if(!value) return;
            rt_kprintf("ldt[%d] -> %d\n", i, value);
        };
    });
    return RT_EOK;
}

INIT_APP_EXPORT(init_load_detector);