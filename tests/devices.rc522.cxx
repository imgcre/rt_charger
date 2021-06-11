#include <devices/rc522.hxx>
#include <rtthread.h>

static int init_rc522() {
    auto rc522 = Preset::Rc522::get();
    rc522->oCardId += [](auto value) {
        if(value) {
            rt_kprintf("card: %s\n", value->c_str());
        } else {
            rt_kprintf("card removed\n");
        }
    };
    rt_kprintf("Swipe card to test rc522\n");
    return RT_EOK;
}

INIT_APP_EXPORT(init_rc522);