#include <rtthread.h>
#include <stdexcept>
#include "test.hxx"
#include <exception>

using namespace std;

Test::Test() {
    throw runtime_error{"test will throw"};
}

namespace __cxxabiv1 {
    std::terminate_handler __terminate_handler = []{ while(true); };
}

extern "C"
void run() {
    // rt_kprintf("APP RUNNED\n");
    // try {
    //     rt_kprintf("before exception\n");
    //     Test{};
    //     rt_kprintf("after exception\n");
    // } catch(runtime_error& e) {
    //     rt_kprintf("catch exception: %s\n", e.what());
    // }
    // rt_kprintf("final\n");
}
