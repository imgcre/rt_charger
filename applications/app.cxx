#include <rtthread.h>
#include <stdexcept>
#include <exception>

using namespace std;

namespace __cxxabiv1 {
    terminate_handler __terminate_handler = []{ while(true); };
}

extern "C"
void run() {
    
}
