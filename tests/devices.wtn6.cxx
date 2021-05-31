#include <termios/cmd.hxx>
#include <rtthread.h>
#include <stdexcept>
#include <devices/wtn6.hxx>

using namespace std;

void test_wtn6(int argc, char** argv) {
    auto wtn6 = Preset::Wtn6::get();
    Cmd{argc, argv}([&](Cmd& cmd) {
        auto vcode = cmd.get<rt_uint8_t>();
        wtn6->write(vcode);
    });
}

MSH_CMD_EXPORT(test_wtn6, )