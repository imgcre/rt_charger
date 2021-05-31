#include "os.hxx"
#include <os/task.hxx>

namespace Preset {

template<> const char* Task<Priority::High>::kThread = "high";
template<> const int Task<Priority::High>::kThreadStack = 512;
template<> const int Task<Priority::High>::kThreadTick = 10;

template<> const char* Task<Priority::Middle>::kThread = "middle";
template<> const int Task<Priority::Middle>::kThreadStack = 2500;
template<> const int Task<Priority::Middle>::kThreadTick = 8;

template<> const char* Task<Priority::Low>::kThread = "low";
template<> const int Task<Priority::Low>::kThreadStack = 2048;
template<> const int Task<Priority::Low>::kThreadTick = 10;

}