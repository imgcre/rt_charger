#include <termios/cmd.hxx>
#include <stdexcept>
#include <rtthread.h>
using namespace std;

void test_cmd_except(int argc, char** argv) {
    Cmd{argc, argv}([](auto& cmd){
        throw runtime_error{"some err"};
    });
}

MSH_CMD_EXPORT(test_cmd_except, );

