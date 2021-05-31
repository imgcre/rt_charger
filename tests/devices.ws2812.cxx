#include <devices/ws2812.hxx>
#include <rtthread.h>
#include <termios/cmd.hxx>

static void test_ws2812(int argc, char** argv) {
    auto ws2812 = Preset::Ws2812::get();
    Cmd{argc, argv}([&](Cmd& cmd) {
        auto i = cmd.get<int>();
        auto r = cmd.get<rt_uint8_t>();
        auto g = cmd.get<rt_uint8_t>();
        auto b = cmd.get<rt_uint8_t>();
        ws2812->getPixel(i) = Colors::Rgb{r, g, b};
        ws2812->flush();
    });
}

MSH_CMD_EXPORT(test_ws2812, );
