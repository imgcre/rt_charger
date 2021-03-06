#include <devices/relay.hxx>
#include <termios/cmd.hxx>
#include <utilities/mp.hxx>
#include <configs/bsp.hxx>
#include <string>

using namespace std;

static void relay(int argc, char** argv) {
    Cmd{argc, argv}([&](Cmd& cmd){
        auto port = cmd.get<Port>();
        auto relay = Port::to<Preset::Relay>(port);
        cmd.select({
            {"on", [&]{
                relay->value = Relay::Value::On;
            }},
            {"off", [&]{
                relay->value = Relay::Value::Off;
            }},
        });
    });
}

MSH_CMD_EXPORT(relay, );