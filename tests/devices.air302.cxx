#include <rtthread.h>
#include <string>
#include <devices/air302.hxx>
#include <termios/cmd.hxx>
#include <devices/air302/basic.hxx>

using namespace std;

static int init_test_air302() {
    auto air302 = Preset::Air302::get();
    air302->init();
    rt_kprintf("air302 init OK!\n");
    return RT_EOK;
}

INIT_APP_EXPORT(init_test_air302);

static void test_air_302(int argc, char** argv) {
    auto air302 = Preset::Air302::get();
    Cmd{argc, argv}([&](Cmd& cmd){
        cmd.select<string>({
            {"basic", [&]{
                auto basic = air302->make<Air302N::Basic>();
                cmd.select<string>({
                    {"rssi", [&]{
                        auto rssi = basic->getRssi();
                        rt_kprintf("rssi: %d\n", rssi);
                    }}
                });
            }}
        });
    });
}

MSH_CMD_EXPORT(test_air_302, );

static void test_air_302_make() {
    try {
        auto air302 = Preset::Air302::get();
        rt_kprintf("try create the first inst\n");
        auto inst1 = air302->make<Air302N::Basic>();
        rt_kprintf("try create the second inst\n");
        auto inst2 = air302->make<Air302N::Basic>();
    } catch(const exception& e) {
        rt_kprintf("err: %s\n", e.what());
    }
    

}

MSH_CMD_EXPORT(test_air_302_make, );
