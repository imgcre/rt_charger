#include <os/event_allocator.hxx>
#include <rtthread.h>
#include <termios/cmd.hxx>
#include <array>

using namespace std;

static void event_allocator(int argc, char** argv) {
    Cmd{argc, argv}([](Cmd& cmd) {
        cmd.select({
            {"alloc", [&]{
                static auto e1 = EventAllocator::alloc();
                {
                    static auto e2 = EventAllocator::alloc();
                    static auto e3 = EventAllocator::alloc();
                    static auto e4 = EventAllocator::alloc();
                    static auto e5 = EventAllocator::alloc();
                    static auto e6 = EventAllocator::alloc();
                    static auto e7 = EventAllocator::alloc();
                    static auto e8 = EventAllocator::alloc();
                    static auto e9 = EventAllocator::alloc();
                    static auto e10 = EventAllocator::alloc();
                    static auto e11 = EventAllocator::alloc();
                    static auto e12 = EventAllocator::alloc();
                    static auto e13 = EventAllocator::alloc();
                    static auto e14 = EventAllocator::alloc();
                    static auto e15 = EventAllocator::alloc();
                    static auto e16 = EventAllocator::alloc();
                    static auto e17 = EventAllocator::alloc();
                    static auto e18 = EventAllocator::alloc();
                    static auto e19 = EventAllocator::alloc();
                    static auto e20 = EventAllocator::alloc();
                    static auto e21 = EventAllocator::alloc();
                    static auto e22 = EventAllocator::alloc();
                    static auto e23 = EventAllocator::alloc();
                    static auto e24 = EventAllocator::alloc();
                    static auto e25 = EventAllocator::alloc();
                    static auto e26 = EventAllocator::alloc();
                    static auto e27 = EventAllocator::alloc();
                    static auto e28 = EventAllocator::alloc();
                    static auto e29 = EventAllocator::alloc();
                    static auto e30 = EventAllocator::alloc();
                    static auto e31 = EventAllocator::alloc();
                    static auto e32 = EventAllocator::alloc();
                    static auto e33 = EventAllocator::alloc();
                }
                {
                    // auto e3 = EventAllocator::alloc();
                }
                
            }},
            {"comp", [&]{
                auto arr = array<rt_event_t, 33>{};
                for(auto& e: arr) {
                    e = rt_event_create("ces", RT_IPC_FLAG_FIFO);
                } 
            }},
        });
    });
}

MSH_CMD_EXPORT(event_allocator, );