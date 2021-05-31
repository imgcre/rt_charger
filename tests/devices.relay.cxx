#include <devices/relay.hxx>
#include <termios/cmd.hxx>
#include <utilities/mp.hxx>
#include <configs/bsp.hxx>
#include <string>

using namespace std;

static void test_relay(int argc, char** argv) {
    Cmd{argc, argv}([&](Cmd& cmd){
        auto iport = cmd.get<int>();
        cmd.assertV(0 <= iport && iport < Config::Bsp::kPortNum, "port out of range");
        magic_switch<Config::Bsp::kPortNum>{}([&](auto v) {
            auto relay = Preset::Relay<decltype(v)::value>::get();
            cmd.select<string>({
                {"on", [&]{
                    relay->value = Relay::Value::On;
                }},
                {"off", [&]{
                    relay->value = Relay::Value::Off;
                }},
            });
        }, iport);
        
    });
}

MSH_CMD_EXPORT(test_relay, );